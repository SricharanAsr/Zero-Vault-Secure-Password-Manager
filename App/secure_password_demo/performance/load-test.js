import http from 'k6/http';
import { check, sleep } from 'k6';

export const options = {
    stages: [
        { duration: '30s', target: 50 },  // Ramp up to 50 users over 30 seconds
        { duration: '1m', target: 50 },   // Stay at 50 users for 1 minute
        { duration: '30s', target: 0 },   // Ramp down to 0 users
    ],
    thresholds: {
        // 95% of requests should be below 2000ms ( Epic 6 Requirement: <2s target )
        http_req_duration: ['p(95)<2000'],
        // 99% of requests must complete without error
        http_req_failed: ['rate<0.01'],
    },
};

const BASE_URL = __ENV.API_BASE_URL || 'http://localhost:5000/api';

export default function () {
    // Simulate fetching a large vault
    const loginRes = http.post(`${BASE_URL}/auth/login`, JSON.stringify({
        username: 'test_user_' + __VU, // Virtual User ID
        password: 'password123'
    }), {
        headers: { 'Content-Type': 'application/json' },
    });

    check(loginRes, {
        'logged in successfully': (r) => r.status === 200,
    });

    if (loginRes.status === 200) {
        const token = loginRes.json('token');

        // Request vault data (simulating retrieving up to 10k entries)
        const vaultRes = http.get(`${BASE_URL}/vault`, {
            headers: { 'Authorization': `Bearer ${token}` },
        });

        check(vaultRes, {
            'vault retrieved': (r) => r.status === 200,
            'latency acceptable': (r) => r.timings.duration < 2000,
        });
    }

    sleep(1); // Think time
}
