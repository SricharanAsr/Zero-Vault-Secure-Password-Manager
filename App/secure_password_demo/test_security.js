const http = require('http');

function testEndpoint(name, path, method, headers, body) {
    return new Promise((resolve) => {
        const data = body ? JSON.stringify(body) : '';
        const reqHeaders = {
            ...headers,
            ...(body ? { 'Content-Type': 'application/json', 'Content-Length': data.length } : {})
        };

        const req = http.request({
            hostname: 'localhost', port: 5000, path, method, headers: reqHeaders
        }, res => {
            let resData = '';
            res.on('data', d => resData += d);
            res.on('end', () => resolve({ statusCode: res.statusCode, data: resData }));
        });

        req.on('error', e => resolve({ error: e.message }));
        if (body) req.write(data);
        req.end();
    });
}

(async () => {
    console.log("=== Security Integration Verification ===");

    // 1. Test Browser Fallback (Regular login with missing headers)
    process.stdout.write("1. Testing Browser Fallbacks (Missing Headers): ");
    const res1 = await testEndpoint('Browser Login', '/api/auth/login', 'POST', {}, { email: 'fake@example.com', password: 'password' });
    if (res1.statusCode === 401 && res1.data.includes('Invalid credentials')) {
        console.log("PASS (gracefully rejected by DB, not C++ Engine DENY)");
    } else {
        console.log(`FAIL\n   Output: ${res1.statusCode} - ${res1.data}`);
    }

    // 2. Test STEP_UP Policy (Medium Risk)
    process.stdout.write("2. Testing STEP_UP Enforcement (Custom Headers): ");
    const res2 = await testEndpoint('Untrusted Device', '/api/auth/login', 'POST',
        { 'x-device-trusted': 'false', 'x-secure-boot': '1' },
        { email: 'fake@example.com', password: 'password' }
    );
    if (res2.statusCode === 401 && res2.data.includes('stepUp')) {
        console.log("PASS (C++ Engine fired STEP_UP for untrusted device)");
    } else {
        console.log(`FAIL\n   Output: ${res2.statusCode} - ${res2.data}`);
    }

    // 3. Test DENY Policy (High Risk)
    process.stdout.write("3. Testing DENY Enforcement (Missing Secure Boot): ");
    const res3 = await testEndpoint('No Secure Boot', '/api/auth/login', 'POST',
        { 'x-device-trusted': 'true', 'x-secure-boot': '0' },
        { email: 'fake@example.com', password: 'password' }
    );
    // Remember validation in MAPPER coerces to missing if false.
    if (res3.statusCode === 403 && res3.data.includes('Access denied')) {
        console.log("PASS (C++ Engine successfully DENIED missing secure boot)");
    } else {
        console.log(`FAIL\n   Output: ${res3.statusCode} - ${res3.data}`);
    }

    // 4. Test Audit Log Verification
    process.stdout.write("4. Testing Audit Hash Chain Verification: ");
    const res4 = await testEndpoint('Audit Verify', '/api/security/audit/verify', 'GET', {}, null);
    if (res4.statusCode === 200 && res4.data.includes('verified successfully')) {
        console.log("PASS (C++ audit chain is valid and verified via Node Addon)");
    } else {
        console.log(`FAIL\n   Output: ${res4.statusCode} - ${res4.data}`);
    }

})();
