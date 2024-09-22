require('dotenv').config();
const express = require('express');
const bitsoBalancesRouter = require('./routes/bitsoBalances');

const app = express();
const PORT = process.env.PORT || 3001;

app.use(bitsoBalancesRouter);

app.listen(PORT, () => console.log(`Server running on port ${PORT}`));