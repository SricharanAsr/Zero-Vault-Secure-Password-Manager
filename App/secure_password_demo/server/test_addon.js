const riskEngine = require('./build/Release/risk_engine');

console.log("Testing Risk Engine Node Addon...\n");

function testEvaluation(scenarioName, signals) {
    console.log(`--- ${scenarioName} ---`);
    console.log(`Signals: ${JSON.stringify(signals)}`);
    try {
        const result = riskEngine.evaluate(signals);
        console.log(`Result: ${result}\n`);
    } catch (e) {
        console.error(`Error: ${e.message}\n`);
    }
}

// Scenario 1: Trusted Device, Low Failures, Secure Boot
testEvaluation("Scenario 1: Low Risk (Allow)", {
    secure_boot: 1,
    failure_count: 0,
    device_trusted: 1
});

// Scenario 2: High Failure Count
testEvaluation("Scenario 2: High Risk (Deny)", {
    secure_boot: 1,
    failure_count: 15,
    device_trusted: 0
});

// Scenario 3: Untrusted device, some failures
testEvaluation("Scenario 3: Medium Risk (Step Up)", {
    secure_boot: 1,
    failure_count: 3,
    device_trusted: 0
});
