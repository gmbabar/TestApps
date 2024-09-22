const express = require('express');
const crypto = require('crypto');
const https = require('https');

const router = express.Router();

// Bitso API credentials
const BITSO_API_KEY = process.env.BITSO_API_KEY;
const BITSO_API_SECRET = process.env.BITSO_API_SECRET;

function httpsRequest(options, data) {
  return new Promise((resolve, reject) => {
    const req = https.request(options, (res) => {
      let body = '';
      res.on('data', (chunk) => body += chunk);
      res.on('end', () => {
        console.log('Response status:', res.statusCode);
        // console.log('Response headers:', res.headers);
        // console.log('Response body:', body);
        
        if (res.statusCode >= 200 && res.statusCode < 300) {
          resolve(JSON.parse(body));
        } else {
          reject(new Error(`Request failed with status code ${res.statusCode}: ${body}`));
        }
      });
    });

    req.on('error', (error) => reject(error));

    if (data) {
      req.write(data);
    }
    req.end();
  });
}

router.get('/api/bitso-balances', async (req, res) => {
  try {
    // console.log('API Key:', BITSO_API_KEY);
    // console.log('API Secret:', BITSO_API_SECRET ? 'Set' : 'Not set');

    const httpMethod = 'GET';
    const requestPath = '/v3/balance/';
    const requestBody = '';
    // const nonce = new Date().getTime();
    const nonce = Math.floor(new Date().getTime() / 1000);
    // const message = nonce.toString();
    const message = nonce + httpMethod + requestPath + requestBody;
    const signature = crypto
      .createHmac('sha256', BITSO_API_SECRET)
      .update(message)
      .digest('hex');

    const authHeader = `Bitso ${BITSO_API_KEY}:${nonce}:${signature}`;
    // console.log('Auth Header:', authHeader);

    const options = {
      hostname: 'api-stage.bitso.com',
      port: 443,
      path: '/v3/balance/',
      method: 'GET',
      headers: {
        'Authorization': authHeader,
        'Content-Type': 'application/json',
      },
    };

    // console.log('Request options:', JSON.stringify(options, null, 2));

    const data = await httpsRequest(options);

    if (data.success) {
      const formattedBalances = data.payload.balances
        .filter(balance => parseFloat(balance.total) > 0)
        .map(balance => ({
          currency: balance.currency,
          balance: balance.total,
          lastUpdated: new Date().toISOString()
        }));

      res.json(formattedBalances);
    } else {
      throw new Error('Failed to fetch balances from Bitso');
    }
  } catch (error) {
    console.error('Error fetching Bitso balances:', error);
    res.status(500).json({ error: 'Internal server error', details: error.message });
  }
});

module.exports = router;