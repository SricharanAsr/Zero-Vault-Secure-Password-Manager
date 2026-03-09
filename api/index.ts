const express = require('express');
const cors = require('cors');
const { createClient } = require('@supabase/supabase-js');
const bcrypt = require('bcryptjs');
const jwt = require('jsonwebtoken');

const app = express();
app.use(cors());
app.use(express.json());

// --- BAKED CREDENTIALS ---
const supabaseUrl = process.env.SUPABASE_URL || 'https://ymmguoxdmvphexggnypo.supabase.co';
const supabaseKey = process.env.SUPABASE_ANON_KEY || 'eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6InltbWd1b3hkbXZwaGV4Z2dueXBvIiwicm9sZSI6InNlcnZpY2Vfcm9sZSIsImlhdCI6MTc3MjIwNTk0NCwiZXhwIjoyMDg3NzgxOTQ0fQ.MEFCqig2lv44Lsajuh-2Vi2VAEQDj-21YLOEdVS7cYU';
const JWT_SECRET = process.env.JWT_SECRET || 'zerovault_jwt_secret_key_2024';

const supabase = createClient(supabaseUrl, supabaseKey);

// Global Error Handler to ensure JSON response
app.use((err, req, res, next) => {
    console.error('[BRIDGE ERROR]', err);
    res.status(500).json({ error: 'Bridge internal error', details: err.message });
});

// --- AUTH ROUTES ---
app.post('/api/auth/register', async (req, res) => {
    try {
        const { email, password, displayName } = req.body;
        console.log('[BRIDGE] Registering:', email);
        const hashedPassword = await bcrypt.hash(password, 10);
        const { data: user, error } = await supabase.from('users').insert([{ email, password_hash: hashedPassword }]).select().single();
        if (error) return res.status(400).json({ error: error.message });
        const token = jwt.sign({ userId: user.id }, JWT_SECRET, { expiresIn: '7d' });
        res.status(201).json({ token, user: { id: user.id, email: user.email, displayName } });
    } catch (e) {
        res.status(500).json({ error: 'Registration failed', details: e.message });
    }
});

app.post('/api/auth/login', async (req, res) => {
    try {
        const { email, password } = req.body;
        console.log('[BRIDGE] Login attempt:', email);
        const { data: user } = await supabase.from('users').select('*').eq('email', email).maybeSingle();
        if (!user) return res.status(401).json({ error: 'Invalid credentials' });
        const isMatch = await bcrypt.compare(password, user.password_hash);
        if (!isMatch) return res.status(401).json({ error: 'Invalid credentials' });
        const token = jwt.sign({ userId: user.id }, JWT_SECRET, { expiresIn: '7d' });
        res.json({ token, user: { id: user.id, email: user.email } });
    } catch (e) {
        res.status(500).json({ error: 'Login failed', details: e.message });
    }
});

app.get('/api/auth/me', async (req, res) => {
    const token = req.headers.authorization?.split(' ')[1];
    if (!token) return res.status(401).json({ error: 'Unauthorized' });
    try {
        const decoded = jwt.verify(token, JWT_SECRET);
        const { data: user } = await supabase.from('users').select('*').eq('id', decoded.userId).single();
        res.json(user);
    } catch (e) {
        res.status(401).json({ error: 'Invalid token' });
    }
});

// --- VAULT ROUTES ---
app.get('/api/vault', async (req, res) => {
    res.json({ items: [], lastSync: new Date().toISOString() });
});

// Fallback
app.all('/api/*', (req, res) => {
    res.status(404).json({ error: 'Endpoint not found in direct bridge' });
});

module.exports = app;
