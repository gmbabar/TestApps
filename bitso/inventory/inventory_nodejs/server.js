const express = require('express');
const axios = require('axios');
const cors = require('cors');
const crypto = require('crypto');  // Needed for API authentication
require('dotenv').config(); // To load environment variables

const app = express();

app.use(express.static(__dirname + '/'));
app.use(cors());    // Ensures that clients from different domains can make requests 
app.use(express.json());    // Ensures that the JSON payload is parsed and made available as req.body.


// Timestamp formatter
function getFormattedTimestamp() {
    const date = new Date();
    return `${date.getFullYear()}${String(date.getMonth() + 1).padStart(2, '0')}${String(date.getDate()).padStart(2, '0')}-${String(date.getHours()).padStart(2, '0')}:${String(date.getMinutes()).padStart(2, '0')}:${String(date.getSeconds()).padStart(2, '0')}`;
}

// Middleware to log requests and responses
app.use((req, res, next) => {
    const start = Date.now();
    const timestamp = getFormattedTimestamp();

    // Log the request
    console.log(`[${timestamp}] CLT-IN : ${req.method} ${req.url}`);

    // Capture the response body by overriding res.send
    const originalSend = res.send;
    res.send = function (body) {
        const duration = Date.now() - start;
        const responseStatus = res.statusCode;

        // Log the response
        console.log(`[${timestamp}] CLT-OUT: ${req.method} ${req.url} - Response: ${responseStatus}, Duration: ${duration}ms`);
        console.log(`[${timestamp}] CLT-OUT: body:`, typeof body === 'object' ? JSON.stringify(body) : body);

        // Call the original res.send
        originalSend.call(this, body);
    };

    next();
});

// Add Axios request and response interceptors for logging
axios.interceptors.request.use(request => {
    const timestamp = `[${getFormattedTimestamp()}] SRV-OUT:`;
    const method_url = `Method: ${request.method.toUpperCase()} URL: ${request.url}`;
    const headers = `Headers: ${JSON.stringify(request.headers)}`;
    const body = request.data ? JSON.stringify(request.data) : '';
    console.log(`${timestamp} ${method_url} ${headers} ${body}`);
    return request;
}, error => {
    console.error(`[${getFormattedTimestamp()}] SRV-OUT: Error in request: ${error}`);
    return Promise.reject(error);
});

axios.interceptors.response.use(response => {
    const timestamp = `[${getFormattedTimestamp()}] SRV-IN :`;
    const status = `Status: ${response.status}`;
    const data = `Response Data: ${JSON.stringify(response.data)}`;
    console.log(`${timestamp} ${status} ${data}`);
    return response;
}, error => {
    if (error.response) {
        // Coinbase API returned an error response
        const timestamp = `[${getFormattedTimestamp()}] SRV-IN: Error response:`;
        const status = `Status: ${error.response.status}`;
        const data = `Error Data: ${JSON.stringify(error.response.data)}`;
        console.log(`${timestamp} ${status} ${data}`);
    } else {
        // Other errors (e.g., network errors)
        console.error(`[${getFormattedTimestamp()}] Error in Axios request: ${error.message}`);
    }
    return Promise.reject(error);
});

// Coinbase API keys
// SANDBOX:    'https://api-public.sandbox.exchange.coinbase.com'
// PRODUCTION: 'https://api.exchange.coinbase.com'
const COINBASE_API_URL = process.env.COINBASE_API_URL;
const API_KEY = process.env.COINBASE_API_KEY;
const API_PASSPHRASE = process.env.COINBASE_API_PASSPHRASE;
const API_SECRET = process.env.COINBASE_API_SECRET; // Secret for signing requests

// Bitso API keys from environment variables
const BITSO_API_KEY = process.env.BITSO_API_KEY;
const BITSO_API_SECRET = process.env.BITSO_API_SECRET;
const BITSO_API_URL = process.env.BITSO_API_URL;
// SANDBOX:    'https://api-stage.bitso.com'
// PRODUCTION: 'https://api.bitso.com'

// Helper function to create a nonce
function getNonce() {
    return Math.floor(new Date().getTime() / 1000);
}

// Helper function to generate the signature
function generateBitsoSignature(httpMethod, requestPath, nonce, apiKey, apiSecret, requestBody = '') {
    const message = nonce + httpMethod + requestPath + requestBody;
    return crypto.createHmac('sha256', apiSecret).update(message).digest('hex');
}

// Function to fetch open orders from Bitso
async function fetchBitsoOpenOrders() {
    const nonce = getNonce();
    const requestPath = '/v3/open_orders/';
    const method = 'GET';
    const body = '';  // GET requests have an empty body

    // Generate the signature
    const signature = generateBitsoSignature(method, requestPath, nonce, BITSO_API_KEY, BITSO_API_SECRET, body);

    try {
        const response = await axios.get(BITSO_API_URL + requestPath, {
            headers: {
                'Authorization': `Bitso ${BITSO_API_KEY}:${nonce}:${signature}`,
                'Content-Type': 'application/json'
            }
        });

        const orders = response.data.payload;

        // Process and map orders to desired format
        const processedOrders = orders.map(order => ({
            OrderId: order.oid,
            Symbol: order.book,
            Size: order.original_amount,
            Side: order.side,
            Status: order.status,
            Price: order.price,
            UnFilledQty: order.unfilled_amount,  // Assuming this corresponds to filled amount
            TIF: order.time_in_force
        }));

        return processedOrders;
    } catch (error) {
        console.error('Error fetching Bitso open orders:', error);
        return [];
    }
}

// Function to place a new order on Bitso
async function placeBitsoOrder(symbol, price, size, side) {
    const nonce = getNonce();
    const requestPath = '/v3/orders/';
    const method = 'POST';

    // Create the body with the parameters received from the client
    const body = JSON.stringify({
        book: symbol.replace('-','_').toLowerCase(),
        price: price,
        major: size,
        side: side,
        type: 'limit'  // You can adjust the order type as needed
    });

    // Generate the signature
    const signature = generateBitsoSignature(method, requestPath, nonce, BITSO_API_KEY, BITSO_API_SECRET, body);

    try {
        const response = await axios.post(BITSO_API_URL + requestPath, body, {
            headers: {
                'Authorization': `Bitso ${BITSO_API_KEY}:${nonce}:${signature}`,
                'Content-Type': 'application/json'
            }
        });

        // Return the response from the Bitso API
        return {
            success: true,
            message: 'Order placed successfully',
            data: response.data.payload
        };
    } catch (error) {
        console.error('Error placing Bitso order:', error);

        // Handle error and return appropriate response
        return {
            success: false,
            message: 'Error placing order',
            error: error.response ? error.response.data : error.message
        };
    }
}

// Route to place a new Bitso order
app.post('/api/bitso/orders', async (req, res) => {
    const { symbol, price, size, side } = req.body;

    // Validate the input
    if (!symbol || !price || !size || !side) {
        return res.status(400).send('Missing parameters: symbol, price, size, and side are required');
    }

    try {
        const result = await placeBitsoOrder(symbol, price, size, side);

        if (result.success) {
            res.json(result);
        } else {
            res.status(500).json(result);
        }
    } catch (error) {
        console.error('Error in placing order:', error);
        res.status(500).json({error: `Error placing order - ${error}`});
    }
});


// Helper function to cancel Bitso order
async function cancelBitsoOrder(orderId) {
    try {
        const method = 'DELETE';
        const nonce = getNonce();
        const requestPath = `/api/v3/orders/${orderId}/`;
        const body = '';  // GET requests have an empty body
    
        // Generate the signature
        const signature = generateBitsoSignature(method, requestPath, nonce, BITSO_API_KEY, BITSO_API_SECRET, body);
    
        const response = await axios.delete(BITSO_API_URL + requestPath, {
            headers: {
                'Authorization': `Bitso ${BITSO_API_KEY}:${nonce}:${signature}`,
                'Content-Type': 'application/json'
            }
        });
    
        return response.data;
    } catch (error) {
        console.error('Error cancelling order:', error);
        throw error;
    }
}

// Route to fetch Bitso open orders
app.get('/api/bitso/orders', async (req, res) => {
    const orders = await fetchBitsoOpenOrders();
    res.json(orders);
});

// API endpoint to cancel an order
app.delete('/api/bitso/orders/:orderId', async (req, res) => {
    const { orderId } = req.params;
    if (!orderId) {
        return res.status(400).send('OrderId is required');
    }

    try {
        const result = await cancelBitsoOrder(orderId);
        res.json({ message: 'Order cancelled successfully', result });
    } catch (error) {
        res.status(500).send('Error cancelling order');
    }
});


// Function to fetch balances from Bitso
async function fetchBitsoBalance() {
    const nonce = getNonce();
    const requestPath = '/v3/balance/';
    const method = 'GET';
    const body = '';  // GET requests have an empty body

    // Generate the signature
    const signature = generateBitsoSignature(method, requestPath, nonce, BITSO_API_KEY, BITSO_API_SECRET, body);

    try {
        const response = await axios.get(BITSO_API_URL + requestPath, {
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
function getCoinbaseSignature(timestamp, method, requestPath, body) {
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

// Cache for storing account IDs
const accountIdCache = {};

// Function to fetch account ID for a specific instrument (e.g., BTC) with caching
async function getAccountIdForInstrument(instrument) {
    // Check if accountId is already in the cache
    if (accountIdCache[instrument]) {
        console.log(`Cache hit: Account ID for ${instrument}`);
        return accountIdCache[instrument];  // Return cached accountId
    }

    console.log(`Cache miss: Fetching Account ID for ${instrument} from Coinbase`);

    const timestamp = Date.now() / 1000;
    const requestPath = `/accounts`;
    const method = 'GET';
    const signature = getCoinbaseSignature(timestamp, method, requestPath, '');

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
            }
        });

        // Find the account ID for the specified instrument (e.g., 'BTC')
        const account = response.data.find(acc => acc.currency === instrument);
        if (account) {
            // Cache the account ID for future use
            accountIdCache[instrument] = account.id;
            return account.id; // Return the account ID for the instrument
        } else {
            throw new Error(`No account found for instrument ${instrument}`);
        }
    } catch (error) {
        console.error('Error fetching account ID:', error);
        throw error;
    }
}

// Function to fetch balance for a specific instrument (by account ID)
async function getBalanceForInstrument(instrument) {
    try {
        const accountId = await getAccountIdForInstrument(instrument); // Cached function
        const timestamp = Date.now() / 1000;
        const requestPath = `/accounts/${accountId}`;
        const method = 'GET';
        const signature = getCoinbaseSignature(timestamp, method, requestPath, '');

        const response = await axios({
            method,
            url: `${COINBASE_API_URL}${requestPath}`,
            headers: {
                'CB-ACCESS-KEY': API_KEY,
                'CB-ACCESS-SIGN': signature,
                'CB-ACCESS-TIMESTAMP': timestamp,
                'CB-ACCESS-PASSPHRASE': API_PASSPHRASE,
                'Content-Type': 'application/json'
            }
        });

        return {
            currency: response.data.currency,
            balance: response.data.balance,
            available: response.data.available
        };
    } catch (error) {
        console.error(`Error fetching balance for ${instrument}:`, error);
        throw error;
    }
}

// Route to fetch balance for a specific instrument
app.get('/api/balance/:instrument', async (req, res) => {
    const { instrument } = req.params; // Example: BTC, ETH

    try {
        const balance = await getBalanceForInstrument(instrument);
        res.json(balance);
    } catch (error) {
        res.status(500).json({ error: `Failed to fetch balance for ${instrument}` });
    }
});


// Helper function to fetch active orders from Coinbase API
async function fetchCoinbaseOrders() {
    const timestamp = Date.now() / 1000;
    const requestPath = `/orders`;
    const method = 'GET';

    const signature = getCoinbaseSignature(timestamp, method, requestPath, '');

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
        const signature = getCoinbaseSignature(timestamp, method, requestPath, body);

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
        const signature = getCoinbaseSignature(timestamp, method, requestPath, '');

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
async function fetchCoinbaseFills() {
    const timestamp = Date.now() / 1000;
    const requestPath = `/fills`;
    const method = 'GET';
    const body = '';  // GET request has no body

    const signature = getCoinbaseSignature(timestamp, method, requestPath, body);

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
async function fetchCoinbaseFillsByCurrency(product_id) {
    const timestamp = Date.now() / 1000;
    const requestPath = `/fills?product_id=${product_id}`;
    const method = 'GET';
    const body = '';  // GET request has no body

    const signature = getCoinbaseSignature(timestamp, method, requestPath, body);

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
        const fills = await fetchCoinbaseFills();
        res.json(fills);
    } catch (error) {
        res.status(500).send('Error fetching all fills');
    }
});

// Route to get fills for a specific currency pair
app.get('/api/coinbase/fills/:currencyPair', async (req, res) => {
    const { currencyPair } = req.params;

    try {
        const fills = await fetchCoinbaseFillsByCurrency(currencyPair);
        res.json(fills);
    } catch (error) {
        res.status(500).send(`Error fetching fills for ${currencyPair}`);
    }
});

// Start server
const PORT = process.env.PORT || 3000;
app.listen(PORT, () => console.log(`Server running on port ${PORT}`));
