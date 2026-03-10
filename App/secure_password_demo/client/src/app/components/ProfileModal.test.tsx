import { render, screen, fireEvent } from '@testing-library/react';
import { beforeEach, describe, expect, it, vi } from 'vitest';
import ProfileModal from './ProfileModal';
import { useAuth } from '@/app/contexts/AuthContext';
import { useVault } from '@/app/contexts/VaultContext';
import { useToast } from '@/app/contexts/ToastContext';

// Standard mock setup for context providers
vi.mock('@/app/contexts/AuthContext', () => ({
    useAuth: vi.fn(),
}));

vi.mock('@/app/contexts/VaultContext', () => ({
    useVault: vi.fn(),
}));

vi.mock('@/app/contexts/ToastContext', () => ({
    useToast: vi.fn(),
}));

describe('ProfileModal', () => {
    const mockOnClose = vi.fn();
    const mockShowToast = vi.fn();

    beforeEach(() => {
        vi.clearAllMocks();

        Reflect.set(window, 'localStorage', {
            getItem: vi.fn(() => null),
            setItem: vi.fn(),
            clear: vi.fn(),
            removeItem: vi.fn(),
        });

        // Set default mocked return values
        (useToast as any).mockReturnValue({ showToast: mockShowToast });
        (useAuth as any).mockReturnValue({
            user: {
                id: '1',
                email: 'test@example.com',
                displayName: 'Test User',
                createdAt: '2025-01-01T00:00:00.000Z',
                lastLoginAt: '2025-01-02T00:00:00.000Z',
                loginCount: 5,
            }
        });
        (useVault as any).mockReturnValue({
            entries: [
                { id: 1, password: 'StrongPassword123!' },
                { id: 2, password: 'weak' }
            ]
        });
    });

    it('renders the ProfileModal with extracted user data', () => {
        render(<ProfileModal isOpen={true} onClose={mockOnClose} />);

        // It extracts "Test" and "User" from "Test User"
        expect(screen.getByDisplayValue('Test')).toBeInTheDocument();
        expect(screen.getByDisplayValue('User')).toBeInTheDocument();
        // It brings the email address
        expect(screen.getAllByDisplayValue('test@example.com').length).toBeGreaterThan(0);
        // It displays Member Since, Last Login, Total Logins
        expect(screen.getByText('5')).toBeInTheDocument(); // total logins
    });

    it('can type into fields to update profile and saves to localStorage', () => {
        render(<ProfileModal isOpen={true} onClose={mockOnClose} />);

        const inputs = screen.getAllByRole('textbox');
        const firstNameInput = inputs[0]; // Assuming it's the first

        fireEvent.change(firstNameInput, { target: { value: 'NewName' } });
        expect(screen.getByDisplayValue('NewName')).toBeInTheDocument();

        const saveBtn = screen.getByRole('button', { name: /save changes/i });
        fireEvent.click(saveBtn);

        // check save effects
        expect(window.localStorage.setItem).toHaveBeenCalledWith(
            'userProfileDetailed',
            expect.stringContaining('NewName')
        );
        expect(mockShowToast).toHaveBeenCalledWith('Profile updated', 'success');
        expect(mockOnClose).toHaveBeenCalled();
    });

    it('loads previously saved data from localStorage if available', () => {
        (window.localStorage.getItem as any).mockReturnValueOnce(JSON.stringify({
            firstName: 'SavedName',
            lastName: 'SavedLast',
            email: 'saved@example.com',
            country: 'Canada',
            username: 'saveduser',
            avatarUrl: '',
            bannerUrl: '',
            isPremium: false,
            isEmailVerified: true,
            memberSince: '1 Mar, 2025',
            lastLogin: 'Today',
            totalLogins: 10,
            vaultHealth: 100
        }));

        render(<ProfileModal isOpen={true} onClose={mockOnClose} />);

        expect(screen.getByDisplayValue('SavedName')).toBeInTheDocument();
        expect(screen.getByDisplayValue('SavedLast')).toBeInTheDocument();
        expect(screen.getByDisplayValue('saveduser')).toBeInTheDocument();
        expect(screen.getByDisplayValue('Canada')).toBeInTheDocument();
    });

    it('closes when Cancel is clicked', () => {
        render(<ProfileModal isOpen={true} onClose={mockOnClose} />);

        const cancelBtn = screen.getByRole('button', { name: /cancel/i });
        fireEvent.click(cancelBtn);

        expect(mockOnClose).toHaveBeenCalled();
    });
});
