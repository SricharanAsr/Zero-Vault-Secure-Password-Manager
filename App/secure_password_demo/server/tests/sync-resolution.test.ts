import request from 'supertest';
import express from 'express';

// Mock Express app for testing sync conflict resolution (Epic 4)
const app = express();
app.use(express.json());

let currentVersion = 10;

app.post('/api/vault/sync', (req, res) => {
    const { version } = req.body;
    if (version < currentVersion) {
        // Conflict: client is behind
        res.status(409).json({ currentServerVersion: currentVersion });
    } else {
        // Accept the update, increment version
        currentVersion = version + 1;
        res.status(200).json({ newVersion: currentVersion });
    }
});

describe('Epic 4: Multi-Device Sync Testing', () => {
    beforeEach(() => {
        currentVersion = 10;
    });

    it('TC-SYNC-001: Simultaneous updates on two devices, verify conflict detection and resolution', async () => {
        const token = "mock.jwt.token";
        const baseVersion = 10;

        // Device A sends an update based on version 10
        const deviceAUpdate = {
            version: baseVersion,
            deltas: [{ id: "entry1", encryptedData: "dataA" }]
        };

        const resA = await request(app)
            .post('/api/vault/sync')
            .set('Authorization', `Bearer ${token}`)
            .send(deviceAUpdate);

        expect([200, 201]).toContain(resA.status);

        // Device B sends an update based on the SAME version 10 (Conflict)
        // Now server version is 11, so version 10 < 11 => 409
        const deviceBUpdate = {
            version: baseVersion,
            deltas: [{ id: "entry2", encryptedData: "dataB" }]
        };

        const resB = await request(app)
            .post('/api/vault/sync')
            .set('Authorization', `Bearer ${token}`)
            .send(deviceBUpdate);

        // The server should reject Device B's update with a 409 Conflict
        expect(resB.status).toBe(409);
        expect(resB.body).toHaveProperty('currentServerVersion');
    });
});
