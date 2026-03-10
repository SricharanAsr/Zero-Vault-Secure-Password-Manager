const express = require('express');
const cors = require('cors');
const { createClient } = require('@supabase/supabase-js');
const bcrypt = require('bcryptjs');
const jwt = require('jsonwebtoken');

const app = express();
app.use(cors());
app.use(express.json());

// --- BAKED CREDENTIALS (Environment Variables take precedence) ---
const supabaseUrl = process.env.SUPABASE_URL || 'https://ymmguoxdmvphexggnypo.supabase.co';
const supabaseKey = process.env.SUPABASE_ANON_KEY || 'eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6InltbWd1b3hkbXZwaGV4Z2dueXBvIiwicm9sZSI6InNlcnZpY2Vfcm9sZSIsImlhdCI6MTc3MjIwNTk0NCwiZXhwIjoyMDg3NzgxOTQ0fQ.MEFCqig2lv44Lsajuh-2Vi2VAEQDj-21YLOEdVS7cYU';
const JWT_SECRET = process.env.JWT_SECRET || 'zerovault_jwt_secret_key_2024';

const supabase = createClient(supabaseUrl, supabaseKey);

// --- AUTH MIDDLEWARE ---
const authMiddleware = async (req: any, res: any, next: any) => {
    const token = req.headers.authorization?.split(' ')[1];
    if (!token) return res.status(401).json({ error: 'Unauthorized' });
    try {
        const decoded = jwt.verify(token, JWT_SECRET);
        req.userId = decoded.userId;
        next();
    } catch (e: any) {
        res.status(401).json({ error: 'Invalid token' });
    }
};

// --- AUTH ROUTES ---
app.post('/api/auth/register', async (req: any, res: any) => {
    try {
        const { email, password, displayName } = req.body;
        console.log('[BRIDGE] Registering:', email);

        const { data: existingUser } = await supabase.from('users').select('id').eq('email', email).maybeSingle();
        if (existingUser) return res.status(400).json({ error: 'User already exists' });

        const hashedPassword = await bcrypt.hash(password, 10);
        const { data: user, error } = await supabase.from('users').insert([{ email, password_hash: hashedPassword }]).select().single();
        if (error) return res.status(400).json({ error: error.message });

        await supabase.from('user_sync_state').insert([{ user_id: user.id, vault_version: 0 }]);

        const token = jwt.sign({ userId: user.id }, JWT_SECRET, { expiresIn: '7d' });
        res.status(201).json({ token, user: { id: user.id, email: user.email, displayName } });
    } catch (e: any) {
        res.status(500).json({ error: 'Registration failed', details: e.message });
    }
});

app.post('/api/auth/login', async (req: any, res: any) => {
    try {
        const { email, password } = req.body;
        console.log('[BRIDGE] Login attempt:', email);
        const { data: user } = await supabase.from('users').select('*').eq('email', email).maybeSingle();
        if (!user) return res.status(401).json({ error: 'Invalid credentials' });

        const isMatch = await bcrypt.compare(password, user.password_hash);
        if (!isMatch) return res.status(401).json({ error: 'Invalid credentials' });

        const token = jwt.sign({ userId: user.id }, JWT_SECRET, { expiresIn: '7d' });
        res.json({ token, user: { id: user.id, email: user.email } });
    } catch (e: any) {
        res.status(500).json({ error: 'Login failed', details: e.message });
    }
});

app.get('/api/auth/me', authMiddleware, async (req: any, res: any) => {
    try {
        const { data: user } = await supabase.from('users').select('id, email, created_at').eq('id', req.userId).single();
        res.json(user);
    } catch (e: any) {
        res.status(500).json({ error: 'Failed to fetch user' });
    }
});

// --- VAULT ROUTES ---
app.get('/api/vault', authMiddleware, async (req: any, res: any) => {
    try {
        const userId = req.userId;
        let { data: syncState } = await supabase.from('user_sync_state').select('*').eq('user_id', userId).maybeSingle();

        if (!syncState) {
            const { data: newState } = await supabase.from('user_sync_state').insert([{ user_id: userId, vault_version: 0 }]).select().single();
            syncState = newState;
        }

        const { data: entries } = await supabase.from('vault_entries').select('*').eq('user_id', userId).neq('is_deleted', true);

        const formattedEntries = (entries || []).map((row: any) => ({
            id: Number(row.id),
            version: row.version,
            title: row.title,
            username: row.username,
            password: row.password,
            website: row.website,
            category: row.category,
            isFavorite: row.is_favorite,
            isDeleted: row.is_deleted,
            updatedAt: row.updated_at
        }));

        res.json({
            userId,
            vaultVersion: syncState.vault_version,
            encryptedEntries: formattedEntries,
            lastSyncedAt: syncState.last_synced_at
        });
    } catch (e: any) {
        res.status(500).json({ error: 'Failed to fetch vault' });
    }
});

app.post('/api/vault/sync', authMiddleware, async (req: any, res: any) => {
    try {
        const userId = req.userId;
        const { baseVersion, added, updated, deleted } = req.body;

        const { data: syncState } = await supabase.from('user_sync_state').select('*').eq('user_id', userId).maybeSingle();
        if (!syncState) return res.status(404).json({ error: 'Sync state not found' });

        if (baseVersion !== syncState.vault_version) {
            const { data: entries } = await supabase.from('vault_entries').select('*').eq('user_id', userId);
            return res.status(409).json({
                error: 'Sync Conflict',
                vaultVersion: syncState.vault_version,
                encryptedEntries: entries
            });
        }

        const nextVersion = syncState.vault_version + 1;
        const upserts: any[] = [];

        [...(added || []), ...(updated || [])].forEach((entry: any) => {
            upserts.push({
                id: entry.id,
                user_id: userId,
                version: nextVersion,
                title: entry.title,
                username: entry.username,
                password: entry.password,
                website: entry.website,
                category: entry.category,
                is_favorite: entry.isFavorite,
                is_deleted: entry.isDeleted || false,
                updated_at: entry.updatedAt
            });
        });

        if (upserts.length > 0) {
            await supabase.from('vault_entries').upsert(upserts, { onConflict: 'id' });
        }

        if (deleted && deleted.length > 0) {
            await supabase.from('vault_entries').delete().in('id', deleted).eq('user_id', userId);
        }

        const now = new Date().toISOString();
        await supabase.from('user_sync_state').update({ vault_version: nextVersion, last_synced_at: now }).eq('user_id', userId);

        const { data: finalEntries } = await supabase.from('vault_entries').select('*').eq('user_id', userId);

        res.json({
            success: true,
            vaultVersion: nextVersion,
            entries: finalEntries,
            lastSyncedAt: now
        });
    } catch (e: any) {
        res.status(500).json({ error: 'Sync failed', details: e.message });
    }
});

// --- DEVICE ROUTES ---
app.get('/api/devices', authMiddleware, async (req: any, res: any) => {
    try {
        const { data: devices } = await supabase.from('devices').select('*').eq('user_id', req.userId);
        res.json({ devices: devices || [] });
    } catch (e: any) {
        res.status(500).json({ error: 'Failed to fetch devices' });
    }
});

app.post('/api/devices/register', authMiddleware, async (req: any, res: any) => {
    try {
        const { device_id, device_name } = req.body;
        const { data: existing } = await supabase.from('devices').select('id').eq('user_id', req.userId).eq('device_id', device_id).maybeSingle();

        if (existing) {
            await supabase.from('devices').update({ last_seen_at: new Date().toISOString() }).eq('id', existing.id);
            return res.json({ message: 'Device already registered' });
        }

        await supabase.from('devices').insert([{
            user_id: req.userId,
            device_id,
            device_name: device_name || 'Unknown Device',
            is_trusted: true,
            registered_at: new Date().toISOString(),
            last_seen_at: new Date().toISOString()
        }]);

        res.status(201).json({ message: 'Device registered' });
    } catch (e: any) {
        res.status(500).json({ error: 'Failed to register device' });
    }
});

// Fallback
app.all('/api/*path', (req: any, res: any) => {
    res.status(404).json({ error: 'Endpoint not found' });
});

module.exports = app;
