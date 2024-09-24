const express = require('express');
const axios = require('axios');
const cors = require('cors');
const crypto = require('crypto');  // Needed for API authentication
require('dotenv').config(); // To load environment variables

const app = express();
app.use(cors());    // Ensures that clients from different domains can make requests 
app.use(express.json());    // Ensures that the JSON payload is parsed and made available as req.body.

// Function to format the timestamp
function getFormattedTimestamp() {
    const date = new Date();
    const YYYY = date.getFullYear();
    const MM = String(date.getMonth() + 1).padStart(2, '0');
    const DD = String(date.getDate()).padStart(2, '0');
    const HH = String(date.getHours()).padStart(2, '0');
    const mm = String(date.getMinutes()).padStart(2, '0');
    const SS = String(date.getSeconds()).padStart(2, '0');

    return `${YYYY}${MM}${DD}-${HH}:${mm}:${SS}`;
}

// Middleware to log requests and responses
app.use((req, res, next) => {
    const start = Date.now();
    const timestamp = getFormattedTimestamp();

    // Log the request
    console.log(`[${timestamp}] ${req.method} ${req.url} - Incoming request`);

    // Capture the response body by overriding res.send
    const originalSend = res.send;
    res.send = function (body) {
        const duration = Date.now() - start;
        const responseStatus = res.statusCode;

        // Log the response
        console.log(`[${timestamp}] ${req.method} ${req.url} - Response: ${responseStatus}, Duration: ${duration}ms`);
        console.log(`[${timestamp}] Response body:`, typeof body === 'object' ? JSON.stringify(body) : body);

        // Call the original res.send
        originalSend.call(this, body);
    };

    next();
});

// Coinbase API keys
const COINBASE_API_URL = 'https://api-public.sandbox.exchange.coinbase.com';
const API_KEY = process.env.COINBASE_API_KEY;
const API_PASSPHRASE = process.env.COINBASE_API_PASSPHRASE;
const API_SECRET = process.env.COINBASE_API_SECRET; // Secret for signing requests

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
    const what = timestamp + method + requestPath + body;
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
            url: `${COINBASE_API_URL}${requestPath}`,
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

// Helper function to fetch active orders from Coinbase API
async function fetchCoinbaseOrders() {
    const timestamp = Date.now() / 1000;
    const requestPath = `/orders`;
    const method = 'GET';

    const signature = getSignature(timestamp, method, requestPath, '');

    try {
        const response = await axios({
            method,
            url: `${COINBASE_API_URL}${requestPath}`,
            headers: {
                'CB-ACCESS-KEY': API_KEY,
                'CB-ACCESS-SIGN': signature,
                'CB-ACCESS-TIMESTAMP': timestamp,
                'CB-ACCESS-PASSPHRASE': API_PASSPHRASE,
                'Content-Type': 'application/json'
            },
        });

        console.log('RESPONSE:', response);

        const orders = response.data.map(order => ({
            OrderId: order.id,
            Symbol: order.product_id,
            Size: order.size,
            Price: order.price,
            Side: order.side,
            OrderType: order.type,
            PostOnly: order.post_only,
            Status: order.status,
            TIF: order.time_in_force,
            FilledSize: order.filled_size
        }));

        return orders;
    } catch (error) {
        console.error('Error fetching Coinbase orders:', error);
        throw error;
    }
}

// Helper function to place a new order
async function placeNewOrder(symbol, side, size, price) {
    try {
        const method = 'POST';
        const requestPath = `/orders`;
        const timestamp = Date.now() / 1000;
        const body = JSON.stringify({
            type: 'limit',
            product_id: symbol,
            side: side,
            size: size,
            price: price,
            post_only: true, // save cost
            time_in_force: 'GTC'
        });
        const signature = getSignature(timestamp, method, requestPath, body);

        const response = await axios({
            method,
            url: `${COINBASE_API_URL}${requestPath}`,
            headers: {
                'CB-ACCESS-KEY': API_KEY,
                'CB-ACCESS-SIGN': signature,
                'CB-ACCESS-TIMESTAMP': timestamp,
                'CB-ACCESS-PASSPHRASE': API_PASSPHRASE,
                'Content-Type': 'application/json'
            },
            data: body
        });
        return response.data;
    } catch (error) {
        // console.error('Error placing new order:', error);
        // throw error;

        // Check if the error response has a status and data field
        if (error.response) {
            const { status, data } = error.response;

            if (status === 400) {
                // Handle 400 status code - return meaningful error message
                console.error(`Error placing new order: ${data.message}`);
                return { error: data.message || 'Invalid request parameters', status: status };
            } else {
                // Handle other status codes if necessary
                console.error(`Error placing new order: ${data.message}`);
                return { error: `Error ${status}: ${data.message}`, status: status };
            }
        } else {
            // If no response or status is available (network error, etc.)
            console.error('Error placing new order:', error.message);
            return { error: 'Failed to place order due to network or server issue' };
        }
    }
}

// Helper function to cancel an order
async function cancelOrder(orderId) {
    try {
        const method = 'DELETE';
        const requestPath = `/orders/${orderId}`;
        const timestamp = Date.now() / 1000;
        const signature = getSignature(timestamp, method, requestPath, '');

        const response = await axios({
            method,
            url: `${COINBASE_API_URL}${requestPath}`,
            headers: {
                'CB-ACCESS-KEY': API_KEY,
                'CB-ACCESS-SIGN': signature,
                'CB-ACCESS-TIMESTAMP': timestamp,
                'CB-ACCESS-PASSPHRASE': API_PASSPHRASE,
                'Content-Type': 'application/json',
            },
        });

        // const response = await axios.delete(`${COINBASE_API_URL}${requestPath}`, {
        //     headers: {
        //         'CB-ACCESS-KEY': API_KEY,
        //         'CB-ACCESS-SIGN': signature,
        //         'CB-ACCESS-TIMESTAMP': timestamp,
        //         'CB-ACCESS-PASSPHRASE': API_PASSPHRASE,
        //         'Content-Type': 'application/json'
        //     }
        // });

        return response.data;
    } catch (error) {
        console.error('Error cancelling order:', error);
        throw error;
    }
}


// API endpoint to get active Coinbase orders
app.get('/api/coinbase/orders', async (req, res) => {
    try {
        orders = await fetchCoinbaseOrders();
        res.json(orders);
    } catch (error) {
        console.log("Error sending response:", error);
        res.status(500).send('Error fetching orders');
    }
});

// API endpoint to cancel an order
app.delete('/api/coinbase/orders/:orderId', async (req, res) => {
    const { orderId } = req.params;
    if (!orderId) {
        return res.status(400).send('OrderId is required');
    }

    try {
        const result = await cancelOrder(orderId);
        res.json({ message: 'Order cancelled successfully', result });
    } catch (error) {
        res.status(500).send('Error cancelling order');
    }
});

// API endpoint to place a new order
app.post('/api/coinbase/orders', async (req, res) => {
    const { symbol, side, size, price } = req.body;

    // Check if any required parameter is missing
    if (!symbol || !side || !size || !price) {
        return res.status(400).send('Missing parameters: symbol, size, and price are required');
    }

    try {
        const order = await placeNewOrder(symbol, side, size, price);

        // If there's an error returned from placeNewOrder, return it as a response
        if (order.error) {
            return res.status(order.status).json({ error: order.error });
        }

        res.json({ message: 'Order placed successfully', order });
    } catch (error) {
        res.status(500).json({ error: 'Failed to place order due to network or server issue' });
    }
});

// Function to fetch all fills from Coinbase
async function fetchAllFills() {
    const timestamp = Date.now() / 1000;
    const requestPath = `/fills`;
    const method = 'GET';
    const body = '';  // GET request has no body

    const signature = getSignature(timestamp, method, requestPath, body);

    try {
        const response = await axios({
            method,
            url: `${COINBASE_API_URL}${requestPath}`,
            headers: {
                'CB-ACCESS-KEY': API_KEY,
                'CB-ACCESS-SIGN': signature,
                'CB-ACCESS-TIMESTAMP': timestamp,
                'CB-ACCESS-PASSPHRASE': API_PASSPHRASE,
                'Content-Type': 'application/json'
            },
        });

        // Map response to desired format
        const fills = response.data.map(fill => ({
            TradeID: fill.trade_id,
            Product: fill.product_id,
            Price: fill.price,
            Size: fill.size,
            Fee: fill.fee
        }));

        return fills;
    } catch (error) {
        console.error('Error fetching all fills:', error);
        throw error;
    }
}

// Function to fetch fills for a specific currency pair from Coinbase
async function fetchFillsByCurrency(product_id) {
    const timestamp = Date.now() / 1000;
    const requestPath = `/fills?product_id=${product_id}`;
    const method = 'GET';
    const body = '';  // GET request has no body

    const signature = getSignature(timestamp, method, requestPath, body);

    try {
        const response = await axios({
            method,
            url: `${COINBASE_API_URL}${requestPath}`,
            headers: {
                'CB-ACCESS-KEY': API_KEY,
                'CB-ACCESS-SIGN': signature,
                'CB-ACCESS-TIMESTAMP': timestamp,
                'CB-ACCESS-PASSPHRASE': API_PASSPHRASE,
                'Content-Type': 'application/json'
            },
        });

        // Map response to desired format
        const fills = response.data.map(fill => ({
            TradeID: fill.trade_id,
            Product: fill.product_id,
            Price: fill.price,
            Size: fill.size,
            Fee: fill.fee
        }));

        return fills;
    } catch (error) {
        console.error(`Error fetching fills for ${product_id}:`, error);
        throw error;
    }
}

// Route to get all fills
app.get('/api/coinbase/fills', async (req, res) => {
    try {
        const fills = await fetchAllFills();
        res.json(fills);
    } catch (error) {
        res.status(500).send('Error fetching all fills');
    }
});

// Route to get fills for a specific currency pair
app.get('/api/coinbase/fills/:currencyPair', async (req, res) => {
    const { currencyPair } = req.params;

    try {
        const fills = await fetchFillsByCurrency(currencyPair);
        res.json(fills);
    } catch (error) {
        res.status(500).send(`Error fetching fills for ${currencyPair}`);
    }
});

// Start server
const PORT = process.env.PORT || 3000;
app.listen(PORT, () => console.log(`Server running on port ${PORT}`));
