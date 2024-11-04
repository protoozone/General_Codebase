// The overall performance of this script was benchmarked against other players/developers
//this script typically acquires $3.6b on the testing environment, indicating a ~47% optimisation
//compared to the highest current achiever

//TODO: Add recursive target acquisition, improve batching framework, scale deployable threads to 
//exactly the theoretical 400,000 script maximum

/** @param {NS} ns */
export async function main(ns) {
  const HOME_THRESHOLD = 2000;
  ns.disableLog("ALL");
  ns.tail();

  let storedIncome = [0];

  // Provides a list of all servers
  function scan_all_servers() {
    // Initialise a set to keep track of all scanned servers
    let servers = new Set();
    // Initialise a queue for BFS traversal
    let queue = ["home"];

    while (queue.length > 0) {
      let current = queue.shift();  // Dequeue the next server

      if (!servers.has(current)) {
        servers.add(current);  // Mark the server as seen

        // Get the list of connected servers
        let neighbors = ns.scan(current);

        // Enqueue all unvisited connected servers
        for (let neighbour of neighbours) {
          if (!servers.has(neighbour)) {
            queue.push(neighbour);
          }
        }
      }
    }

    // Convert Set to Array and return it
    return Array.from(servers);
  }

  // Finds the best target server based on money/sec
  function score_target(target) {
    // Clone the server object to avoid issues with direct modification
    let mock = Object.assign({}, ns.getServer(target)); 

    // Modify the cloned object
    mock.hackDifficulty = mock.minDifficulty;  // Use the actual server's minDifficulty
    mock.moneyAvailable = mock.moneyMax;  // Set moneyAvailable to the server's maximum

    // Retrieve times using the ns.formulas.hacking functions
    let hackT = ns.formulas.hacking.hackTime(mock, ns.getPlayer());
    let growT = ns.formulas.hacking.growTime(mock, ns.getPlayer());
    let weakT = ns.formulas.hacking.weakenTime(mock, ns.getPlayer());

    // Calculate hacking proportion and value
    let hackProp = ns.formulas.hacking.hackPercent(mock, ns.getPlayer());
    let hackVal = mock.moneyMax * hackProp;

    // Calculate maximum cycle time
    let buffTime = Math.max(hackT, growT, weakT);

    // Return value that represents money/sec
    return hackVal / buffTime;
  }

  // Returns best target for hgw cycle
  async function acquire_best_target(candidates) {
    let mainTarget = "lll";
    let mainScore = 0;
    for (let target of candidates) {
      if (!(ns.hasRootAccess(target))) {
        if (ns.getHackingLevel() >= ns.getServerRequiredHackingLevel(target)) {
          ns.run("nuker.js", 1, target);
          await ns.sleep(20);
        }
      }

      if (!(ns.hasRootAccess(target))) {
        continue;
      }
      if (score_target(target) > mainScore) {
        mainScore = score_target(target);
        mainTarget = target;
      }
    }
    return mainTarget;
  }
  
  // Returns a dictionary of all servers and their threadcount for hgw's,
  // as well as a total amount of threads runnable across the net 
  async function run_the_open_net(list) {
    let dict = {};
    let total = 0;
    for (let serv of list) {
      ns.scp("hackerTerm.js", serv, "home");
      ns.scp("growerTerm.js", serv, "home");
      ns.scp("weakenerTerm.js", serv, "home");
      if (ns.getHackingLevel() < ns.getServerRequiredHackingLevel(serv)) {
        continue;
      }

      // Skip servers that start with "hacknet"
      if (serv.startsWith("hacknet")) {
        continue;
      }

      if (serv == "home") {
        const hgwThreads = Math.floor((ns.getServerMaxRam(serv)-ns.getServerUsedRam(serv)-HOME_THRESHOLD)/1.85);
        total += hgwThreads;
        dict[serv] = hgwThreads;
      }

      const hgwThreads = Math.floor((ns.getServerMaxRam(serv)-ns.getServerUsedRam(serv))/1.85);
      total += hgwThreads;
      dict[serv] = hgwThreads;
      await ns.sleep(10);
    }

    return [dict, total];
  }

  // Analyses the Hack-Grow-Weaken proportion
  function hgw_analysis(target) {
    let mock = Object.assign({}, ns.getServer(target)); 
    mock.moneyAvailable = mock.moneyMax;
    mock.hackDifficulty = mock.minDifficulty;

    // Hack Analysis
    let hackVal = ns.hackAnalyze(mock.hostname); // Fraction of money hacked per thread
    let hackPercent = 0.1; // Let's say you want to hack 10% of the target's total money
    // let hackThreads = Math.ceil(hackPercent / hackVal); // How many threads needed to hack 10% of the money
    let hackThreads = hackPercent / hackVal; // How many threads needed to hack 10% of the money

    // Hack security increase
    let hackSecIncrease = ns.hackAnalyzeSecurity(hackThreads, mock.hostname);

    // Grow Analysis
    let moneyStolen = hackPercent; // How much money you've stolen
    let growReq = ns.growthAnalyze(mock.hostname, 1 / (1 - moneyStolen), 1); // How many grow threads needed to restore the stolen amount
    // let growThreads = Math.ceil(growReq); // Ceiling the grow threads
    let growThreads = growReq; // Ceiling the grow threads

    // Grow security increase
    let growSecIncrease = ns.growthAnalyzeSecurity(growThreads);

    // Weaken Analysis
    let weakenDif = ns.weakenAnalyze(1, 1); // Amount security is decreased per weaken thread
    let totalSecIncrease = hackSecIncrease + growSecIncrease; // Total security increase from hacking and growing
    // let weakenThreads = Math.ceil(totalSecIncrease / weakenDif); // Weaken threads needed to offset the security increase
    let weakenThreads = totalSecIncrease / weakenDif; // Weaken threads needed to offset the security increase

    return [hackThreads, growThreads, weakenThreads];
  }

  // Deploys HGW across dictionary of servers
  async function deploy_batches(target, dict_arr) {
    let hgw = hgw_analysis(target);
    let deployedScripts = [];
    let minHgw = Math.min(...hgw);
    let hThreads = Math.floor(hgw[0] / minHgw);
    let gThreads = Math.floor(hgw[1]);
    let wThreads = Math.ceil(1 + hgw[2]);
    let pids2 = [];

    await ns.sleep(100);

    // Reduces a server's security and increases its money if unable to hack at present
    while (hThreads == Infinity) {
      if (!(pids2.length > 4)) {
        pids2.push(ns.exec("growerTerm.js", "home", ns.args[0], target, ns.getWeakenTime(target)-ns.getGrowTime(target)));
        pids2.push(ns.exec("weakenerTerm.js", "home", ns.args[0], target));
      }

      // Resets hgw for new security level
      await ns.sleep(20);
      hgw = hgw_analysis(target);
      minHgw = Math.min(...hgw);
      hThreads = Math.floor(hgw[0] / minHgw);
      gThreads = Math.ceil(hgw[1]);
      wThreads = Math.ceil(1 + hgw[2]);

    }

    // Deletes all other hgw batches
    ns.exec("cleaner.js", "home");

    // Returns hgw proportion
    ns.print("Scaled Hack, Grow, Weaken thread prop: " + hThreads + ", " + gThreads + ", " + wThreads);

    let totalUsableSpace = dict_arr[1];
    let deployableThreads = 0;
    let dictInd = 0;
    let pids = 0;

    dict_arr = dict_arr[0];
    let keys = Object.keys(dict_arr);
    
    // Reusable function to deploy threads for a given task (hack, grow, or weaken)
    async function deployTask(script, threadsNeeded, buffer) {
      let deployableThreads;
      let powOffset = 1;
      while (threadsNeeded > 0) {        
        // await (ns.sleep(0));
        if (dictInd >= keys.length) {
          ns.print("Ran out of servers for deployment");
          return false;
        }

        if (dict_arr[dictInd] < 1) {
          dictInd += 1;
          continue;
        }

        deployableThreads = Math.min(threadsNeeded, dict_arr[keys[dictInd]]);

        // If no threads can be deployed, move to the next server
        if (deployableThreads < 1) {
          dictInd += 1; // Move to the next server
          continue; // Continue to check the next server
        }

        // Scale up hgw densities in the case we will run too many scripts for VM memory
        let scaler = 4;
        if (keys[dictInd].startsWith("Server")) {
          if (ns.getServerMaxRam(keys[dictInd]) > (1048576/(16*scaler))) {
            powOffset = ns.getServerMaxRam(keys[dictInd])/(1048576/(16*scaler));
            deployableThreads = deployableThreads*powOffset;
          }
        }

        if (keys[dictInd].startsWith("home")) {
          if (ns.getServerMaxRam(keys[dictInd]) > 1048576/(2*scaler)) {
            powOffset = ns.getServerMaxRam(keys[dictInd])/(1048576/(2*scaler));
            deployableThreads = deployableThreads*powOffset;
          }
        }

        // Deploy threads with scaled back hacks to the initial hgw proportion
        if (script != "hackerTerm.js") {
          if (deployableThreads < 1) {
            deployableThreads = 1;
          }
          pids = ns.exec(script, keys[dictInd], {threads:deployableThreads,  temporary:true}, target, buffer)
        } else {
          if (deployableThreads < 1) {
            deployableThreads = 1;
          }
          pids = ns.exec(script, keys[dictInd], {threads:deployableThreads,  temporary:true}, target, buffer, (hgw[0]), deployableThreads);
        }

        // Store the list of script IDs from deployed hack
        if (pids != 0) {
          deployedScripts.push(pids);
        }
        
        // Reduces deployable threads on server and iterates to next if all deployable threads are used up
        dict_arr[keys[dictInd]] -= deployableThreads;
        threadsNeeded -= deployableThreads;
        totalUsableSpace -= deployableThreads;

        if (dict_arr[keys[dictInd]] <= 0) {
          dictInd += 1;
        }
      }
      return true;
    }

    // While there are still threads to deploy, deploy them
    while (totalUsableSpace > 0) {
      // Deploy hack threads
      if (!(await deployTask("hackerTerm.js", hThreads, ns.getWeakenTime(target)-ns.getHackTime(target)))) break;

      // Deploy grow threads
      if (!(await deployTask("growerTerm.js", gThreads, ns.getWeakenTime(target)-ns.getGrowTime(target)))) break;

      // Deploy weaken threads
      if (!(await deployTask("weakenerTerm.js", wThreads, 0))) break;
    }

    // Return back the deployed script IDs
    return deployedScripts;
  }

  // Waits until the end of the last deployed script before cleaning the current batches
  async function batch_monitor(deployedScripts) {
    while (ns.isRunning(deployedScripts[deployedScripts.length-1])) {
      await ns.sleep(20);
      continue;
    }
    await ns.sleep(200);
    ns.exec("cleaner.js", "home", 1);
  }

  // Log for debugging batches and deployments
  function log_output() {
    ns.clearLog();
    ns.print("Last Batch Incomes are: ");

    // Create a copy of the last 9 elements (if available)
    let storedIncomeCopy = storedIncome.slice(-9); // Get the last 9 elements
    let newIncome = ns.getRunningScript("trueHandler.js", "home").onlineMoneyMade;
    
    // Add the new income to the stored income array
    storedIncome.push(newIncome);
    
    // Keep only the last 10 elements in storedIncome
    if (storedIncome.length > 10) {
      storedIncome.shift(); // Remove the oldest element if we exceed 10
    }
      
    // Print differences between incomes
    for (let i = 1; i < storedIncome.length; i++) {
      ns.print("\t" + i + ": $" + (storedIncome[i]-storedIncome[i-1]));
    }
  }

  // Retrieve a tree of the greater web
  let targets = scan_all_servers();

  // Indefinitely loop target acquisition, deployment, and monitoring
  while (true) {
    let focus = await acquire_best_target(targets);
    let deployedBatches = await deploy_batches(focus, await run_the_open_net(targets));
    await batch_monitor(deployedBatches);
    log_output();
  }
}
