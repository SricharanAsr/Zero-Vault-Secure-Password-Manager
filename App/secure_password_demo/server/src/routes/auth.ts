import express, { Request, Response } from 'express';
import bcrypt from 'bcryptjs';
import jwt from 'jsonwebtoken';
import { supabase } from '../storage/supabaseClient';
import { authMiddleware, AuthRequest } from '../middleware/auth';

// Import the compiled Risk Engine Addon
const riskEngine = require('../../build/Release/risk_engine');

const router = express.Router();
const JWT_SECRET = process.env.JWT_SECRET || 'fallback_secret';

// POST /api/auth/register
router.post('/register', async (req: Request, res: Response) => {
    try {
        const { email, password, displayName } = req.body;
        console.log('Registration request:', { email, displayName, password: '***' });

        const { data: existingUser } = await supabase
            .from('users')
            .select('id')
            .eq('email', email)
            .maybeSingle();

        if (existingUser) {
            return res.status(400).json({ error: 'User already exists' });
        }

        const hashedPassword = await bcrypt.hash(password, 10);

        const { data: user, error } = await supabase
            .from('users')
            .insert([{ email, password_hash: hashedPassword }])
            .select()
            .single();

        if (error || !user) throw error;

        // Initialize sync state for the user
        await supabase.from('user_sync_state').insert([{ user_id: user.id }]);

        const token = jwt.sign({ userId: user.id }, JWT_SECRET, { expiresIn: '7d' });

        res.status(201).json({
            token,
            user: { id: user.id, email: user.email, displayName }
        });
    } catch (error: any) {
        console.error('Registration error:', error);
        res.status(500).json({ error: 'Server error', details: error.message || error });
    }
});

// POST /api/auth/login
router.post('/login', async (req: Request, res: Response) => {
    try {
        const { email, password } = req.body;
        console.log('Login request:', { email, password: '***' });

        // Fetch user first to get real signal data like failed_login_count
        const { data: user } = await supabase
            .from('users')
            .select('*')
            .eq('email', email)
            .maybeSingle();

        // Collect signals for the Risk Engine Mapper dynamically
        // Since browsers don't natively send device attestations, default to true unless explicitly provided
        const signals = {
            secure_boot: req.headers['x-secure-boot'] ? req.headers['x-secure-boot'] === '1' : true,
            failed_login_count: user ? (user.login_count || 0) : 0,
            device_trusted: req.headers['x-device-trusted'] ? req.headers['x-device-trusted'] === 'true' : true
        };

        const decision = riskEngine.evaluate(signals);

        // Structured Logging for forensic audit and security monitoring
        console.log(JSON.stringify({
            event: "risk_evaluation",
            user: email,
            decision,
            signals
        }));

        if (decision === 'DENY') {
            return res.status(403).json({ error: 'Access denied by security policy' });
        }
        if (decision === 'STEP_UP') {
            // Trigger MFA Flow (In this demo, we return 401 with requiresMFA flag)
            return res.status(401).json({ error: 'MFA required', requiresMFA: true, stepUp: true });
        }

        if (!user) {
            return res.status(401).json({ error: 'Invalid credentials' });
        }

        const isMatch = await bcrypt.compare(password, user.password_hash);
        if (!isMatch) {
            return res.status(401).json({ error: 'Invalid credentials' });
        }

        const token = jwt.sign({ userId: user.id }, JWT_SECRET, { expiresIn: '7d' });

        // US 3.1 & Epic Dynamic Profile: Update login stats
        await supabase
            .from('users')
            .update({
                last_login_at: new Date().toISOString(),
                login_count: (user.login_count || 0) + 1
            })
            .eq('id', user.id);

        res.json({
            token,
            user: {
                id: user.id,
                email: user.email,
                createdAt: user.created_at,
                lastLoginAt: new Date().toISOString(),
                loginCount: (user.login_count || 0) + 1
            }
        });
    } catch (error) {
        console.error('Login error:', error);
        res.status(500).json({ error: 'Server error' });
    }
});

// GET /api/auth/me
router.get('/me', authMiddleware, async (req: AuthRequest, res: Response) => {
    try {
        const { data: user } = await supabase
            .from('users')
            .select('id, email, created_at, last_login_at, login_count')
            .eq('id', req.userId)
            .single();

        if (!user) return res.status(404).json({ error: 'User not found' });
        res.json({
            id: user.id,
            email: user.email,
            createdAt: user.created_at,
            lastLoginAt: user.last_login_at,
            loginCount: user.login_count
        });
    } catch {
        res.status(500).json({ error: 'Server error' });
    }
});

export default router;
