import { test, expect } from '@playwright/test';

test.describe('Epic 2: Cross-Platform Functional Testing', () => {
    test('TC-FUNC-001: Create password on each platform, verify encrypted storage', async ({ page, browserName }) => {
        console.log(`Running on browser: ${browserName}`);

        // Setup - navigate and login to an existing vault
        await page.goto('http://localhost:5173');

        // Assuming a login page is the first thing that shows up
        const loginPassword = page.locator('input[type="password"]');
        if (await loginPassword.isVisible()) {
            await loginPassword.fill('StrongMasterPassword123!');
            await page.click('button:has-text("Unlock")');
        }

        // Create a new entry
        await page.click('button:has-text("Add Password")');
        await page.fill('input[name="url"]', `https://example-${browserName}.com`);
        await page.fill('input[name="username"]', `user_${browserName}`);
        await page.fill('input[name="password"]', `secret_${browserName}`);
        await page.click('button:has-text("Save")');

        // Verification
        // Verify the entry appears in the list
        const entry = page.locator(`text=example-${browserName}.com`);
        await expect(entry).toBeVisible();

        // Note: The actual encrypted storage verification is done in the API tests
        // Here we just verify the client behavior works consistently across Chromium/Firefox/WebKit
    });
});
