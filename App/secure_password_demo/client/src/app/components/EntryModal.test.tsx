import { render, screen, fireEvent } from '@testing-library/react';
import { beforeEach, describe, expect, it, vi } from 'vitest';
import EntryModal from './EntryModal';

describe('EntryModal', () => {
    const mockOnClose = vi.fn();
    const mockOnSave = vi.fn();

    beforeEach(() => {
        vi.clearAllMocks();
    });

    it('renders Add New Entry mode correctly', () => {
        render(
            <EntryModal
                isOpen={true}
                onClose={mockOnClose}
                onSave={mockOnSave}
                mode="add"
            />
        );

        expect(screen.getByText('Add New Entry')).toBeInTheDocument();
        expect(screen.getByPlaceholderText('e.g., Netflix, GitHub')).toBeInTheDocument();
        expect(screen.getByRole('button', { name: /add entry/i })).toBeInTheDocument();
    });

    it('renders Edit Entry mode with existing data', () => {
        render(
            <EntryModal
                isOpen={true}
                onClose={mockOnClose}
                onSave={mockOnSave}
                mode="edit"
                entry={{
                    id: 123,
                    website: 'TestSite',
                    username: 'testuser',
                    password: 'password123',
                    isFavorite: true,
                    version: 1,
                    updatedAt: new Date().toISOString()
                }}
            />
        );

        expect(screen.getByText('Edit Entry')).toBeInTheDocument();
        expect(screen.getByDisplayValue('TestSite')).toBeInTheDocument();
        expect(screen.getByDisplayValue('testuser')).toBeInTheDocument();
        expect(screen.getByDisplayValue('password123')).toBeInTheDocument();
        expect(screen.getByRole('button', { name: /save changes/i })).toBeInTheDocument();
    });

    it('shows validation errors when submitting empty form', () => {
        render(
            <EntryModal
                isOpen={true}
                onClose={mockOnClose}
                onSave={mockOnSave}
                mode="add"
            />
        );

        const submitBtn = screen.getByRole('button', { name: /add entry/i });
        fireEvent.click(submitBtn);

        expect(screen.getByText('Website/App name is required')).toBeInTheDocument();
        expect(screen.getByText('Username/Email is required')).toBeInTheDocument();
        expect(screen.getByText('Password is required')).toBeInTheDocument();
        expect(mockOnSave).not.toHaveBeenCalled();
    });

    it('calls onSave and onClose when valid form is submitted', () => {
        render(
            <EntryModal
                isOpen={true}
                onClose={mockOnClose}
                onSave={mockOnSave}
                mode="add"
            />
        );

        fireEvent.change(screen.getByPlaceholderText('e.g., Netflix, GitHub'), { target: { value: 'MySite' } });
        fireEvent.change(screen.getByPlaceholderText('user@example.com'), { target: { value: 'user1' } });
        fireEvent.change(screen.getByPlaceholderText('••••••••••••'), { target: { value: 'secretpass' } });

        const submitBtn = screen.getByRole('button', { name: /add entry/i });
        fireEvent.click(submitBtn);

        expect(mockOnSave).toHaveBeenCalledWith(expect.objectContaining({
            website: 'MySite',
            username: 'user1',
            password: 'secretpass',
        }));
        expect(mockOnClose).toHaveBeenCalled();
    });

    it('toggles password visibility correctly', () => {
        render(
            <EntryModal
                isOpen={true}
                onClose={mockOnClose}
                onSave={mockOnSave}
                mode="add"
            />
        );

        const passwordInput = screen.getByPlaceholderText('••••••••••••');
        expect(passwordInput).toHaveAttribute('type', 'password');

        // It is the toggler inside the relative div, it has no exact aria-label, but we can find it by button role.
        const toggleBtns = screen.getAllByRole('button');
        const visibilityToggle = toggleBtns.find(btn => btn.className.includes('absolute right-3'));

        if (visibilityToggle) {
            fireEvent.click(visibilityToggle);
            expect(passwordInput).toHaveAttribute('type', 'text');
            fireEvent.click(visibilityToggle);
            expect(passwordInput).toHaveAttribute('type', 'password');
        }
    });

    it('opens category dropdown and selects a category', () => {
        render(
            <EntryModal
                isOpen={true}
                onClose={mockOnClose}
                onSave={mockOnSave}
                mode="add"
            />
        );

        const categoryBtn = screen.getByText('Select category');
        fireEvent.click(categoryBtn);

        const personalCategory = screen.getByText('Personal');
        fireEvent.click(personalCategory);

        expect(screen.queryByText('Select category')).not.toBeInTheDocument();
        expect(screen.getAllByText('Personal').length).toBeGreaterThan(0);
    });
});
