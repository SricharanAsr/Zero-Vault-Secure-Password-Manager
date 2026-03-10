import { render, screen, fireEvent, waitFor } from '@testing-library/react';
import { beforeEach, describe, expect, it, vi } from 'vitest';
import PasswordGenerator from './PasswordGenerator';

describe('PasswordGenerator', () => {
    it('renders the options with default values', () => {
        render(<PasswordGenerator />);

        expect(screen.getByText(/Length/i)).toBeInTheDocument();
        expect(screen.getByDisplayValue('16')).toBeInTheDocument();
        expect(screen.getByText(/Uppercase/i)).toBeInTheDocument();
        expect(screen.getByText(/Lowercase/i)).toBeInTheDocument();
        expect(screen.getByText(/Numbers/i)).toBeInTheDocument();
        expect(screen.getByText(/Symbols/i)).toBeInTheDocument();
    });

    it('generates a new password when clicking the generate button and calls onGenerate callback', async () => {
        const onGenerate = vi.fn();
        render(<PasswordGenerator onGenerate={onGenerate} />);

        const generateBtn = screen.getByRole('button', { name: /generate password/i });
        fireEvent.click(generateBtn);

        await waitFor(() => {
            expect(onGenerate).toHaveBeenCalled();
        });

        const generatedPassword = onGenerate.mock.calls[0][0];
        expect(generatedPassword).toHaveLength(16);
    });

    it('updates length when slider changes', () => {
        render(<PasswordGenerator />);

        const slider = screen.getByRole('slider');
        fireEvent.change(slider, { target: { value: '20' } });

        expect(screen.getByDisplayValue('20')).toBeInTheDocument();
    });

    it('handles copy to clipboard feature', async () => {
        const onGenerate = vi.fn();
        render(<PasswordGenerator onGenerate={onGenerate} />);

        const generateBtn = screen.getByRole('button', { name: /generate password/i });
        fireEvent.click(generateBtn);

        // Mock clipboard
        Object.defineProperty(navigator, 'clipboard', {
            value: {
                writeText: vi.fn().mockImplementation(() => Promise.resolve()),
            },
            configurable: true,
            writable: true
        });

        // After generation, copy button should appear
        const copyBtns = screen.getAllByRole('button');
        // Generate is 1st or 2nd? Let's just find the closest
        fireEvent.click(copyBtns[0]);

        expect(navigator.clipboard.writeText).toHaveBeenCalledWith(expect.any(String));
    });

    it('toggles checkboxes properly', () => {
        render(<PasswordGenerator />);

        const uppercaseCheckbox = screen.getAllByRole('checkbox')[0];

        expect(uppercaseCheckbox).toBeChecked();
        fireEvent.click(uppercaseCheckbox);
        expect(uppercaseCheckbox).not.toBeChecked();
    });
});
