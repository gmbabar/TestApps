const express = require('express');
const axios = require('axios');
const cors = require('cors');
const crypto = require('crypto');  // Needed for API authentication

const app = express();
app.use(cors());

const API_KEY = '47dfc129c1135efb2348d443297aaa0c';
const API_SECRET = 'igIjLcJHQaBCMaYpGoexS2/HQ5jdKjXX7i4qsSAJkdcjb+deOkNHf0LfD9VDg9lGM+jfK5B6zSmyEASB0PYz1A==';
const API_PASSPHRASE = 'bdtvyfvpyu9';


// Bitso API keys from environment variables
const BITSO_API_KEY = process.env.BITSO_API_KEY;
const BITSO_API_SECRET = process.env.BITSO_API_SECRET;

// Helper function to create a nonce
function getNonce() {
    return Math.floor(new Date().getTime() / 1000);
}

// Helper function to generate the signature
function generateBitsoSignature(httpMethod, requestPath, nonce, apiKey, apiSecret, requestBody = '') {
    const message = nonce + httpMethod + requestPath + requestBody;
    return crypto.createHmac('sha256', apiSecret).update(message).digest('hex');
}

// Function to fetch balances from Bitso
async function fetchBitsoBalance() {
    const nonce = getNonce();
    const requestPath = '/v3/balance/';
    const method = 'GET';
    const body = '';  // GET requests have an empty body

    // Generate the signature
    const signature = generateBitsoSignature(method, requestPath, nonce, BITSO_API_KEY, BITSO_API_SECRET, body);

    const aheaders = {
        'Authorization': `Bitso ${BITSO_API_KEY}:${nonce}:${signature}`,
        'Content-Type': 'application/json'
    }

    try {
        const response = await axios.get('https://api-stage.bitso.com' + requestPath, {
            headers: {
                'Authorization': `Bitso ${BITSO_API_KEY}:${nonce}:${signature}`,
                'Content-Type': 'application/json'
            }
        });

        const balances = response.data.payload.balances;

        // Process the balances and structure the data
        const processedBalances = balances.map(balance => ({
            currency: balance.currency.toUpperCase(),
            available: parseFloat(balance.available),
            total: parseFloat(balance.total)
        }));

        return processedBalances;
    } catch (error) {
        console.error('Error fetching Bitso balance:', error);
        return [];
    }
}

// Route to fetch Bitso balances
app.get('/api/bitso/balances', async (req, res) => {
    const balances = await fetchBitsoBalance();
    res.json(balances);
});

app.get('/api/bitso/balances/:currency', async (req, res) => {
    const requestedCurrency = req.params.currency.toUpperCase();
    const balances = await fetchBitsoBalance();

    // Find the balance for the requested currency
    const currencyBalance = balances.find(balance => balance.currency === requestedCurrency);

    if (currencyBalance) {
        res.json(currencyBalance);
    } else {
        res.status(404).json({ error: `Balance for currency ${requestedCurrency} not found` });
    }
});

// Function to sign the Coinbase API request
function getSignature(timestamp, method, requestPath, body) {
    const what = timestamp + method + requestPath + (body ? JSON.stringify(body) : '');
    const key = Buffer.from(API_SECRET, 'base64');
    return crypto.createHmac('sha256', key).update(what).digest('base64');
}

// Route to get top of the book
app.get('/api/book/:pair', async (req, res) => {
    const productId = req.params.pair; // Example: BTC-USD, ETH-USD

    try {
        const response = await axios.get(`https://api.exchange.coinbase.com/products/${productId}/book?level=1`);
        res.json(response.data);
    } catch (error) {
        console.error(error);
        res.status(500).send('>>>Error fetching data from Coinbase API');
    }
});

// Route to get wallet balance
app.get('/api/balance/:currency', async (req, res) => {
    const currency = req.params.currency;
    const timestamp = Date.now() / 1000;
    const requestPath = `/accounts`;
    const method = 'GET';

    const signature = getSignature(timestamp, method, requestPath, '');

    try {
        const response = await axios({
            method,
            url: `https://api-public.sandbox.exchange.coinbase.com${requestPath}`,
            headers: {
                'CB-ACCESS-KEY': API_KEY,
                'CB-ACCESS-SIGN': signature,
                'CB-ACCESS-TIMESTAMP': timestamp,
                'CB-ACCESS-PASSPHRASE': API_PASSPHRASE,
                'Content-Type': 'application/json',
            },
        });

        // Find the balance for the selected currency
        const account = response.data.find(acc => acc.currency === currency);
        if (account) {
            res.json({ balance: account.balance });
        } else {
            res.json({ balance: '0.00' }); // No balance if the account does not exist
        }
    } catch (error) {
        console.error(error);
        res.status(500).send('>>>Error fetching balance from Coinbase API');
    }
});

// Start server
const PORT = process.env.PORT || 3000;
app.listen(PORT, () => console.log(`Server running on port ${PORT}`));
