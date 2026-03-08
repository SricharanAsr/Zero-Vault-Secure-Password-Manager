const fs = require('fs');
const https = require('https');
const path = require('path');

// ==========================================
// QA Touch Configuration
// These should be set in the environment or CI/CD secrets
// ==========================================
const domain = process.env.QATOUCH_DOMAIN || 'zeroknowledgevault';
const apiToken = process.env.QATOUCH_API_TOKEN || '';
const projectKey = process.env.QATOUCH_PROJECT_KEY || 'mPx9';
const testRunId = process.env.QATOUCH_TEST_RUN_ID || '';

const apiUrl = `https://api.qatouch.com/api/v1/testRunResults/status`;

console.log('🔄 Starting QA Touch Integration Sync...');
console.log(`📊 Project: ${projectKey} | Run: ${testRunId} | Domain: ${domain}`);

if (!apiToken || !domain || !testRunId) {
  console.warn('⚠️  WARNING: QA Touch credentials (Token, Domain, or Test Run ID) not fully provided.');
  console.warn('   Test results will be parsed but NOT uploaded to the API.');
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
  if (!apiToken || testResults.length === 0 || !testRunId) {
    console.log(`ℹ️  Skipping API upload. Compiled ${testResults.length} test cases.`);
    if (testResults.length > 0) {
      console.log('Sample result:', JSON.stringify(testResults[0], null, 2));
    }
    return;
  }

  console.log(`🚀 Uploading ${testResults.length} results to QA Touch...`);

  // Transform to QA touch expected payload formatting. 
  const payload = JSON.stringify({
    project: projectKey,
    test_run: testRunId,
    results: testResults
  });

  console.log('📦 API Payload:', payload);

  const options = {
    hostname: 'api.qatouch.com',
    path: '/api/v1/testRunResults/status',
    method: 'POST',
    headers: {
      'Content-Type': 'application/json',
      'Content-Length': Buffer.byteLength(payload),
      'api-token': apiToken,
      'domain': domain
    }
  };

  const req = https.request(options, (res) => {
    let data = '';
    res.on('data', (chunk) => data += chunk);
    res.on('end', () => {
      console.log(`✅ QA Touch Response [${res.statusCode}]`);
      try {
        const parsed = JSON.parse(data);
        console.log('📝 Response Body:', JSON.stringify(parsed, null, 2));
      } catch (e) {
        console.log('📝 Response Body (Raw):', data);
      }
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
