import { test, expect } from '@playwright/test';

test.describe('Epic 7: Usability & Accessibility Testing', () => {
    test('TC-UX-001: Test first-time user can create vault and understand encryption transparency', async ({ page }) => {
        // Navigate to local client URL (assuming standard Vite port 5173 for local testing)
        await page.goto('http://localhost:5173');

        // Wait for the app to load
        await expect(page).toHaveTitle(/ZeroVault/i);

        // Click "Get Started" or similar onboarding step depending on frontend implementation
        // These selectors are estimations and will mature as frontend solidifies.
        const createVaultBtn = page.locator('button:has-text("Create New Vault")');
        if (await createVaultBtn.isVisible()) {
            await createVaultBtn.click();
        }

        // Fill the onboarding form
        await page.fill('input[type="password"]', 'StrongMasterPassword123!');
        await page.fill('input[name="confirmPassword"]', 'StrongMasterPassword123!');

        // Check transparent encryption UI feedback
        const encryptionNotice = page.locator('text=Your master password never leaves this device');
        await expect(encryptionNotice).toBeVisible();

        // Submit
        await page.click('button[type="submit"]');

        // Assert successful vault creation (Dashboard view)
        const dashboardHeader = page.locator('h1:has-text("My Vault")');
        await expect(dashboardHeader).toBeVisible({ timeout: 10000 });
    });
});
