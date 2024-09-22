import React, { useState, useEffect } from 'react';
import axios from 'axios';

const Table = ({ children }) => (
  <table style={{ width: '100%', borderCollapse: 'collapse' }}>
    {children}
  </table>
);

const TableHead = ({ children }) => (
  <thead style={{ backgroundColor: '#f8f8f8' }}>
    {children}
  </thead>
);

const TableBody = ({ children }) => (
  <tbody>
    {children}
  </tbody>
);

const TableRow = ({ children }) => (
  <tr>
    {children}
  </tr>
);

const TableCell = ({ children }) => (
  <td style={{ border: '1px solid #ddd', padding: '8px' }}>
    {children}
  </td>
);

const BitsoBalanceTable = () => {
  const [balances, setBalances] = useState([]);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState(null);

  useEffect(() => {
    const fetchBalances = async () => {
      try {
        const response = await axios.get('/api/bitso-balances');
        setBalances(response.data);
        setLoading(false);
      } catch (err) {
        setError(err.message);
        setLoading(false);
      }
    };

    fetchBalances();
  }, []);

  if (loading) {
    return <div>Loading...</div>;
  }

  if (error) {
    return <div>Error: {error}</div>;
  }

  return (
    <div style={{ padding: '20px' }}>
      <h1 style={{ marginBottom: '20px' }}>Bitso Account Balances</h1>
      <Table>
        <TableHead>
          <TableRow>
            <TableCell>Currency</TableCell>
            <TableCell>Balance</TableCell>
            <TableCell>Last Updated</TableCell>
          </TableRow>
        </TableHead>
        <TableBody>
          {balances.map((balance) => (
            <TableRow key={balance.currency}>
              <TableCell>{balance.currency}</TableCell>
              <TableCell>{balance.balance}</TableCell>
              <TableCell>{new Date(balance.lastUpdated).toLocaleString()}</TableCell>
            </TableRow>
          ))}
        </TableBody>
      </Table>
    </div>
  );
};

export default BitsoBalanceTable;