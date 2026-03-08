const fs = require('fs');
const https = require('https');
const path = require('path');

// ==========================================
// QA Touch Configuration
// These should be set in the environment or CI/CD secrets
// ==========================================
const domain = process.env.QATOUCH_DOMAIN || 'YOUR_DOMAIN'; // e.g., zeroknowledgevault
const apiToken = process.env.QATOUCH_API_TOKEN || 'YOUR_API_TOKEN';
const projectKey = process.env.QATOUCH_PROJECT_KEY || 'YOUR_PROJECT_KEY';
const testRunId = process.env.QATOUCH_TEST_RUN_ID || 'YOUR_TEST_RUN_ID';

const apiUrl = `https://api.qatouch.com/api/v1/testRunResults/status`;

console.log('🔄 Starting QA Touch Integration Sync...');

if (!process.env.QATOUCH_API_TOKEN || !process.env.QATOUCH_DOMAIN) {
  console.warn('⚠️  WARNING: QA Touch API Token or Domain not found in environment.');
  console.warn('   Test results will be parsed but NOT uploaded.');
}

// Map local status strings to QA Touch status IDs 
// Status Maps depend on QA touch settings, usually:
// 1: Passed, 2: Untested, 3: Blocked, 4: Retest, 5: Failed
const statusMap = {
  'passed': 1,
  'failed': 5,
  'skipped': 2,
};

/**
 * Push results to QA Touch using standard HTTPS request
 */
function pushToQATouch(testResults) {
  if (!process.env.QATOUCH_API_TOKEN || testResults.length === 0) {
      console.log(`ℹ️  Skipping API upload. Compiled ${testResults.length} test cases.`);
      return;
  }

  console.log(`🚀 Uploading ${testResults.length} results to QA Touch...`);

  // Transform to QA touch expected payload formatting. 
  // You might need to adjust payload structure based on exact QA Touch API docs
  const payload = JSON.stringify({
    project: projectKey,
    test_run: testRunId,
    results: testResults
  });

  const options = {
    hostname: 'api.qatouch.com',
    path: '/api/v1/testRunResults/status',
    method: 'POST',
    headers: {
      'Content-Type': 'application/json',
      'Content-Length': payload.length,
      'api-token': apiToken,
      'domain': domain
    }
  };

  const req = https.request(options, (res) => {
    let data = '';
    res.on('data', (chunk) => data += chunk);
    res.on('end', () => {
      console.log(`✅ QA Touch Response [${res.statusCode}]: ${data}`);
    });
  });

  req.on('error', (e) => {
    console.error(`❌ QA Touch API Request Failed: ${e.message}`);
  });

  req.write(payload);
  req.end();
}

/**
 * Parse Jest JSON Output
 */
function parseJestResults(filePath) {
  const results = [];
  try {
    if (fs.existsSync(filePath)) {
      const raw = fs.readFileSync(filePath);
      const data = JSON.parse(raw);
      
      data.testResults.forEach(suite => {
        suite.assertionResults.forEach(test => {
           // We try to extract a Case ID from title e.g., "TC-API-001: Confirm master..."
           const caseMatch = test.title.match(/TC-[A-Z]+-\d+/);
           const caseId = caseMatch ? caseMatch[0] : test.title;

           results.push({
             case: caseId,
             status: statusMap[test.status] || 5,
             comments: test.failureMessages ? test.failureMessages.join('\n') : 'Automated Run pass',
           });
        });
      });
      console.log(`✅ Parsed Jest Results from ${filePath}`);
    } else {
      console.log(`⚠️  Jest result file not found: ${filePath}`);
    }
  } catch (err) {
    console.error(`❌ Error parsing Jest file:`, err);
  }
  return results;
}

// ---------------------------------------------------------
// Main Execution Engine
// ---------------------------------------------------------

const allResults = [];

// Aggregate test results. Hardcoded paths for the sprint
// In real-world, you pass these as ARGV
const jestResultsPath = path.resolve(__dirname, '../App/secure_password_demo/server/jest-results.json');
allResults.push(...parseJestResults(jestResultsPath));

// Future implementation area: Parse Playwright results 
// ...

pushToQATouch(allResults);
