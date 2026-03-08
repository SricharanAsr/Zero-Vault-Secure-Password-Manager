import { describe, it, expect, vi, beforeEach } from 'vitest';
import { render, screen, waitFor, act } from '@testing-library/react';
import { VaultProvider, useVault } from '@/app/contexts/VaultContext';
import { ToastProvider } from '@/app/contexts/ToastContext';
import type { ReactNode } from 'react';
import crypto from 'node:crypto';

// Polyfill crypto.randomUUID and BroadcastChannel for JSDOM
if (!globalThis.crypto) {
    (globalThis as any).crypto = {};
}
if (!globalThis.crypto.randomUUID) {
    globalThis.crypto.randomUUID = () => crypto.randomUUID();
}
if (!globalThis.BroadcastChannel) {
    (globalThis as any).BroadcastChannel = class BroadcastChannel {
        name: string;
        constructor(name: string) { this.name = name; }
        postMessage() { }
        close() { }
    };
}

const mockUser = { id: 'test-user' };
const mockLogin = vi.fn();
const mockRegister = vi.fn();
const mockLogout = vi.fn();

// Mock AuthContext
vi.mock('../contexts/AuthContext', async () => {
    const actual = await vi.importActual('../contexts/AuthContext');
    return {
        ...actual,
        useAuth: () => ({
            user: mockUser,
            token: 'test-token',
            login: mockLogin,
            register: mockRegister,
            logout: mockLogout
        }),
        AuthProvider: ({ children }: { children: ReactNode }) => <div>{children}</div>
    };
});

// Mock crypto service to avoid web crypto issues in jsdom
vi.mock('@/shared/sync/crypto.service', () => ({
    cryptoService: {
        encrypt: vi.fn((data) => Promise.resolve(`encrypted-${data}`)),
        decrypt: vi.fn((data) => Promise.resolve(data.replace('encrypted-', ''))),
    }
}));

// Mock fetch — handles GET /vault (initial load) and POST /vault/sync separately
let mockServerState = {
    vaultVersion: 0,
    encryptedEntries: [] as any[],
    lastSyncedAt: null as string | null
};

(globalThis as any).fetch = vi.fn(async (url: string, options?: RequestInit) => {
    const isPost = options?.method === 'POST';

    // Initial GET vault fetch — return current server state
    if (!isPost) {
        return Promise.resolve({
            ok: true,
            status: 200,
            json: () => Promise.resolve({
                vaultVersion: mockServerState.vaultVersion,
                encryptedEntries: mockServerState.encryptedEntries,
                lastSyncedAt: mockServerState.lastSyncedAt,
            }),
        });
    }

    // POST /vault/sync — parse baseVersion, apply deltas and return incremented version
    let responseVersion = mockServerState.vaultVersion;
    try {
        const body = JSON.parse(options?.body as string);
        responseVersion = (body.baseVersion ?? mockServerState.vaultVersion) + 1;

        // Apply added/updated to our mock server state
        if (body.updated) {
            body.updated.forEach((updatedEntry: any) => {
                const idx = mockServerState.encryptedEntries.findIndex(e => e.id === updatedEntry.id);
                if (idx > -1) {
                    mockServerState.encryptedEntries[idx] = updatedEntry;
                } else {
                    mockServerState.encryptedEntries.push(updatedEntry);
                }
            });
        }

        mockServerState.vaultVersion = responseVersion;
        mockServerState.lastSyncedAt = new Date().toISOString();

    } catch (e) {
        console.error('Mock fetch parse error', e);
    }

    return Promise.resolve({
        ok: true,
        status: 200,
        json: () => Promise.resolve({
            success: true,
            vaultVersion: responseVersion,
            lastSyncedAt: mockServerState.lastSyncedAt,
            entries: mockServerState.encryptedEntries
        }),
    });
});
// Assign to window.fetch so VaultContext's fetch() calls use this mock
(globalThis as any).fetch.__isMock = true;

// Test Component to consume context
const TestComponent = () => {
    const { addEntry, deleteEntry, entries, syncStatus, isOnline, vaultVersion } = useVault();

    return (
        <div>
            <div data-testid="sync-status">{syncStatus}</div>
            <div data-testid="vault-version">{vaultVersion}</div>
            <div data-testid="online-status">{isOnline ? 'Online' : 'Offline'}</div>
            <div data-testid="entries-count">{entries.length}</div>
            {/* eslint-disable-next-line @typescript-eslint/no-explicit-any */}
            <button onClick={() => addEntry({ title: 'Test', username: 'user', password: 'pw' } as any)}>
                Add Entry
            </button>
            {entries.length > 0 && (
                <button onClick={() => deleteEntry(entries[0].id)}>Delete First</button>
            )}
        </div>
    );
};

const renderWithProviders = (ui: ReactNode) => {
    return render(
        <ToastProvider>
            <VaultProvider>
                {ui}
            </VaultProvider>
        </ToastProvider>
    );
};

describe('VaultContext Integration', () => {
    beforeEach(() => {
        localStorage.clear();
        vi.clearAllMocks();
        mockServerState = {
            vaultVersion: 0,
            encryptedEntries: [],
            lastSyncedAt: null
        };
    });

    it('should add an entry successfully', async () => {
        renderWithProviders(<TestComponent />);

        // Wait for initialization (initial GET returns version 0)
        await waitFor(() => {
            expect(screen.getByTestId('vault-version')).toHaveTextContent('0');
        }, { timeout: 5000 });

        const btn = screen.getByText('Add Entry');
        await act(async () => {
            btn.click();
        });

        await waitFor(() => {
            expect(screen.getByTestId('entries-count')).toHaveTextContent('1');
        }, { timeout: 5000 });
    });

    it('should mark entry as deleted (tombstone)', async () => {
        renderWithProviders(<TestComponent />);

        // Wait for initialization (initial GET returns version 0)
        await waitFor(() => {
            expect(screen.getByTestId('vault-version')).toHaveTextContent('0');
        }, { timeout: 5000 });

        // Add
        const addBtn = screen.getByText('Add Entry');
        await act(async () => {
            addBtn.click();
        });
        await waitFor(() => expect(screen.getByTestId('entries-count')).toHaveTextContent('1'), { timeout: 5000 });

        // Delete
        const delBtn = screen.getByText('Delete First');
        await act(async () => {
            delBtn.click();
        });

        // Should return to 0 visible entries (context filters deleted)
        await waitFor(() => {
            expect(screen.getByTestId('entries-count')).toHaveTextContent('0');
        }, { timeout: 5000 });
    });

    it('should queue changes to outbox when offline', async () => {
        // Mock offline
        Object.defineProperty(window.navigator, 'onLine', { value: false, configurable: true });
        window.dispatchEvent(new Event('offline'));

        renderWithProviders(<TestComponent />);

        // Verify initial state
        expect(screen.getByTestId('online-status')).toHaveTextContent('Offline');

        // Wait for initialization (initial GET returns version 0)
        await waitFor(() => {
            expect(screen.getByTestId('vault-version')).toHaveTextContent('0');
        }, { timeout: 5000 });

        // Add Entry
        const btn = screen.getByText('Add Entry');
        await act(async () => {
            btn.click();
        });

        await waitFor(() => {
            expect(screen.getByTestId('entries-count')).toHaveTextContent('1');
        });

        // Check LocalStorage for outbox persistence
        await waitFor(() => {
            const outbox = JSON.parse(localStorage.getItem('vault_outbox_test-user') || '[]');
            expect(outbox).toHaveLength(1);
            expect(outbox[0].delta.updated).toHaveLength(1);
        });
    });
});
