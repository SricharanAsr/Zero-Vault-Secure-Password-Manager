import express from 'express';
const app = express();

app.get('/api/test', (req, res) => {
    res.json({
        status: 'ok',
        message: 'Standalone backend bridge is functional!',
        env: {
            hasUrl: !!process.env.SUPABASE_URL,
            hasKey: !!process.env.SUPABASE_ANON_KEY
        }
    });
});

// For registration/login
app.all('/api/auth/*', (req, res) => {
    res.status(503).json({
        error: 'Backend is in maintenance (Minimal mode)',
        details: 'Standalone bridge is active but deep app imports are temporarily disabled for debugging.'
    });
});

export default app;
