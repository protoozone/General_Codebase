// Finds and returns the shortest possible path in a grid
//Inputs: array of binary arrays s.t. x, y = data[x][y], and where 1 represents a blockage, 0 represents empty space
//Output: String of LUDR (Left, Up, Down, Right) instructions to reach destination, or an empty string if no path exists
async function shortestPathInGrid(data) {
  const grid = data;
  const m = grid.length;
  const n = grid[0].length;

  // Directions and corresponding move strings
  const directions = [
    { dir: [1, 0], move: 'D' }, // Down
    { dir: [-1, 0], move: 'U' }, // Up
    { dir: [0, 1], move: 'R' }, // Right
    { dir: [0, -1], move: 'L' } // Left
  ];

  // BFS setup
  const queue = [[0, 0]]; // Start from (0, 0)
  const visited = new Set();
  visited.add("0,0"); // Mark starting point as visited
  const parent = {}; // To reconstruct path

  while (queue.length > 0) {
    const [x, y] = queue.shift();

    // Check if we've reached the destination
    if (x === m - 1 && y === n - 1) {
      // Reconstruct the path
      let path = '';
      let cur = `${x},${y}`;
      while (cur !== "0,0") {
        const [px, py] = parent[cur];
        const dx = px - x; // Calculate difference
        const dy = py - y;
        path = (dx === 1 ? 'D' : dx === -1 ? 'U' : '') +
          (dy === 1 ? 'R' : dy === -1 ? 'L' : '') + path;
        cur = `${px},${py}`;
        x = px; // Move to parent
        y = py; // Update current position
      }
      return path; // Return the constructed path
    }

    // Explore neighbors
    for (const { dir, move } of directions) {
      const newX = x + dir[0];
      const newY = y + dir[1];

      // Check bounds and if cell is unblocked
      if (newX >= 0 && newX < m && newY >= 0 && newY < n && grid[newX][newY] === 0) {
        const newPos = `${newX},${newY}`;
        if (!visited.has(newPos)) {
          visited.add(newPos);
          queue.push([newX, newY]);
          parent[newPos] = [x, y]; // Track parent
        }
      }
    }
  }

  return ""; // No path found
}

// Finds the colouring of a graph with vertices and edge data
//Input: array containing vertices num and edge array of pair arrays
//Output: binary array containing the colouring of the graph (assuming 0 as initial)
async function twoColoring(data) {
  const vertices = data[0];
  const edges = data[1];

  // Initialize the graph as an adjacency list
  const graph = Array.from({ length: vertices }, () => []);
  for (const [u, v] of edges) {
    graph[u].push(v);
    graph[v].push(u); // Since the graph is undirected
  }

  const color = Array(vertices).fill(-1); // -1 indicates uncolored

  // Function to perform BFS for coloring
  const bfs = (start) => {
    const queue = [start];
    color[start] = 0; // Color the starting vertex with color 0

    while (queue.length > 0) {
      const current = queue.shift();

      for (const neighbor of graph[current]) {
        if (color[neighbor] === -1) {
          // Assign the opposite color to the neighbor
          color[neighbor] = 1 - color[current];
          queue.push(neighbor);
        } else if (color[neighbor] === color[current]) {
          // A conflict in coloring means the graph is not bipartite
          return false; // Not bipartite
        }
      }
    }
    return true; // Successfully colored this component
  };

  // Check all vertices to ensure all components are processed
  for (let i = 0; i < vertices; i++) {
    if (color[i] === -1) { // If this vertex is uncolored
      if (!bfs(i)) {
        return []; // Return early if not bipartite
      }
    }
  }

  return color; // Return the color array if bipartite
}

// Finds the optimal amount achievable from trading on stock histories
//Input: array containing a history of stock prices, number of transactions to use
//Output: total profit achievable
async function stockTrader(prices, n) {
  const days = prices.length;

  // Handle edge cases
  if (days === 0 || n === 0) return 0;

  // If n is greater than half the number of days, we can make as many transactions as we want
  if (n >= Math.floor(days / 2)) {
    let totalProfit = 0;
    for (let i = 1; i < days; i++) {
      if (prices[i] > prices[i - 1]) {
        totalProfit += prices[i] - prices[i - 1];
      }
    }
    return totalProfit;
  }

  // Initialize the DP table
  const dp = Array.from({ length: n + 1 }, () => Array(days).fill(0));

  // Fill the DP table
  for (let k = 1; k <= n; k++) {
    let maxDiff = -prices[0]; // Represents the maximum profit we can have by buying at a previous price
    for (let i = 1; i < days; i++) {
      dp[k][i] = Math.max(dp[k][i - 1], prices[i] + maxDiff);
      maxDiff = Math.max(maxDiff, dp[k - 1][i] - prices[i]); // Update maxDiff
    }
  }

  // The answer is the maximum profit with n transactions on the last day
  return dp[n][days - 1];
}

// Finds the number of unique paths in a grid
//Inputs: height and length of the grid
//Output: number of unique paths to bottom right corner of grid
async function uniquePathsGridI(data) {
  const [x, y] = data;

  // Function to calculate factorial
  function factorial(n) {
    if (n === 0 || n === 1) return 1;
    let result = 1;
    for (let i = 2; i <= n; i++) {
      result *= i;
    }
    return result;
  }

  // Using the combinatorial formula C(m+n-2, m-1)
  // m = number of rows (y), n = number of columns (x)
  const m = y; // rows
  const n = x; // columns

  // Special cases
  if (m === 0 || n === 0) return 0; // No paths if there's no grid
  if (m === 1 || n === 1) return 1; // Only one way to move in a single row/column

  // Calculate the number of unique paths
  const paths = factorial(m + n - 2) / (factorial(m - 1) * factorial(n - 1));
  return paths;
}

// Finds the number of unique paths in a grid with obstacles
//Inputs: array of binary arrays s.t. x, y = data[x][y], and where 1 represents a blockage, 0 represents empty space
//Output: number of unique paths to bottom right corner of grid
async function uniquePathsGridII(data) {
  const grid = data;
  const m = grid.length;    // number of rows
  const n = grid[0].length; // number of columns

  // If the starting or ending cell is blocked, return 0
  if (grid[0][0] === 1 || grid[m - 1][n - 1] === 1) {
    return 0;
  }

  // Create a DP table initialized to 0
  const dp = Array.from({ length: m }, () => Array(n).fill(0));
  dp[0][0] = 1; // Starting point

  // Fill the DP table
  for (let i = 0; i < m; i++) {
    for (let j = 0; j < n; j++) {
      if (grid[i][j] === 1) {
        dp[i][j] = 0; // No paths if the cell is blocked
      } else {
        if (i > 0) dp[i][j] += dp[i - 1][j]; // From above
        if (j > 0) dp[i][j] += dp[i][j - 1]; // From the left
      }
    }
  }

  // Return the number of unique paths to the bottom-right corner
  return dp[m - 1][n - 1];
}

// Finds the largest prime factor of a number (Uses sieve up to sqrt(n))
//Inputs: number to find the prime factor of
//Output: largest prime factor
async function findLargestPrimeFactor(data) {
  // Check for special cases
  if (data === 0) return 0; // Treat 0 as prime
  if (data === 1) return 1; // Treat 1 as prime

  // Function to generate all prime numbers up to n using the Sieve of Eratosthenes
  function sieve(n) {
    const isPrime = new Array(n + 1).fill(true);
    const primes = [];
    isPrime[0] = isPrime[1] = false; // 0 and 1 are not prime numbers

    for (let i = 2; i <= n; i++) {
      if (isPrime[i]) {
        primes.push(i);
        for (let j = i * i; j <= n; j += i) {
          isPrime[j] = false; // Mark multiples as non-prime
        }
      }
    }
    return primes;
  }

  const sqrtData = Math.floor(Math.sqrt(data));
  const primes = sieve(sqrtData);
  let largestPrimeFactor = null;

  // Check for prime factors starting from the largest prime
  for (let i = primes.length - 1; i >= 0; i--) {
    const prime = primes[i];

    // Divide data by the prime as long as it's divisible
    while (data % prime === 0) {
      largestPrimeFactor = prime; // Update largest prime factor
      data /= prime; // Divide out the prime factor
    }

    // If data becomes 1, we can break early
    if (data === 1) break;
  }

  // If data is still greater than 1, it is prime itself and larger than any found
  if (data > 1) {
    largestPrimeFactor = data;
  }

  return largestPrimeFactor;
}

// Finds the number of ways a sum can be achieved
//Inputs: target sum, array of numbers that can be used to achieve the target
//Output: number of ways the sum can be achieved
async function totalWaysToSum(target, elements) {
  // Initialize the DP array
  const dp = new Array(target + 1).fill(0);
  dp[0] = 1; // There's one way to achieve the sum of 0

  // Iterate over each element in the array
  for (let element of elements) {
    // Update the dp array from the back to avoid overcounting
    for (let j = element; j <= target; j++) {
      dp[j] += dp[j - element];
    }
  }

  // The answer will be in dp[target]
  return dp[target];
}

// Function to handle the "Array Jumping Game"
async function arrayJumpingGame(data) {
  let range = 0; // Initialize range variable

  // Traverse the array backward
  for (let i = data.length - 1; i >= 0; i--) {
    if (data[i] === 0 || data[i] <= range) {
      range++; // Increase range for a zero or insufficient jump
    } else {
      range = 0; // Reset range if a valid jump is possible
    }
  }

  // If range is 0 at the end, we can clear the array
  return range === 0 ? 1 : 0;
}
