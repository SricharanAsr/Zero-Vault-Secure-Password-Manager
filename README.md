# ZeroVault: Secure Password Manager Extension

ZeroVault is an industry-grade, zero-knowledge password management solution designed for maximum security and seamless multi-device synchronization. Built with a focus on privacy, ZeroVault ensures that your sensitive data never leaves your local environment in a decrypted state.

## 🚀 Core Features & Epics

### 1. Zero-Knowledge Architecture (Epic 1 & 5)
*   **Local-Only Decryption**: Master passwords never leave the client device.
*   **Encrypted Sync**: All data synchronized with the backend is end-to-end encrypted using industry-standard protocols.
*   **Risk Engine Integration**: A native C11-based risk engine detects anomalous patterns to prevent unauthorized access.

### 2. Multi-Device Synchronization (Epic 3 & 4)
*   **Real-time Conflict Resolution**: sophisticated version tracking (409 Conflict detection) ensures data integrity across multiple active devices.
*   **Secure API Layers**: Robust Node.js/Express backend integrated with Supabase PostgreSQL for high-availability storage.

### 3. Cross-Platform Consistency (Epic 2 & 7)
*   **Responsive UI**: Optimized for Web, Desktop, and Mobile environments.
*   **Transparency**: Clear UI indicators confirm local encryption status, providing user confidence in the security model.

### 4. Advanced QA & CI/CD (Epic 6 & 8)
*   **Automated Testing Suite**: Comprehensive coverage using Jest (Integration) and Playwright (E2E).
*   **Performance Benchmarking**: Load testing with k6 ensuring latency remains <800ms for large datasets.
*   **QA Touch Integration**: Automated test reporting to the QA Touch dashboard for centralized quality tracking.

---

## 🛠 Tech Stack

*   **Frontend**: React, Vite, Wouter, Spline (3D Components)
*   **Backend**: Node.js, Express, Supabase (PostgreSQL)
*   **Security Engine**: C11 Native Risk Registry
*   **Testing**: Jest, Playwright, k6
*   **Reporting**: QA Touch API

---

## ⚙️ Setup & Installation

### Prerequisites
*   Node.js (v20.19+ or v22.12+ recommended)
*   npm (v10.5.2+)
*   Supabase Account

### Backend Configuration
1. Navigate to the server directory: `cd App/secure_password_demo/server`
2. Create a `.env` file based on `.env.example`:
   ```env
   SUPABASE_URL=your_supabase_url
   SUPABASE_ANON_KEY=your_supabase_anon_key
   JWT_SECRET=your_jwt_secret
   PORT=5000
   ```
3. Install dependencies: `npm install --ignore-scripts`
4. Start the server: `npm run dev`

### Frontend Configuration
1. Navigate to the client directory: `cd App/secure_password_demo/client`
2. Install dependencies: `npm install`
3. Start the Vite dev server: `npm run dev`

---

## 🧪 Testing & Quality Assurance

### Automated Testing
Run the complete testing suite locally:
```bash
# Run Backend Integration Tests
cd App/secure_password_demo/server
npm test

# Run End-to-End Tests
cd App/secure_password_demo/client
npx playwright test
```

### QA Touch Reporting
To sync local test results with the QA Touch dashboard:
```bash
node scripts/qa-touch-reporter.js
```

---

## 👥 Lead Engineer
**Sricharan A** - Lead QA Automation Engineer

---
*ZeroVault Protection — Because your privacy is non-negotiable.*