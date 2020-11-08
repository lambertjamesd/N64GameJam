
const child_process = require('child_process');
const fs = require('fs');

const spawned = [];
const waitFor = [];

try {
    const lastLaunch = fs.readFileSync('./launch/last_launch.txt', {encoding:'utf8', flag:'r'}); 

    lastLaunch.split(',').forEach(pid => {
        process.kill(+pid);
    });
} catch (error) {

}

if (process.argv[2] == 'cen64') {
    spawned.push(child_process.spawn(
        'node', 
        ['/home/james/libultragdb/proxy/proxy.js', 'localhost:2159', '8080'], 
        {
            detached: true,
        }
    ));

    spawned.push(child_process.spawn(
        '/home/james/cen64/cen64/cen64',
        ['/home/james/cen64/pifdata.bin', process.argv[3]],
        {
            detached: true,
        }
    ));

    waitFor.push(new Promise((resolve) => {
        setTimeout(resolve, 2000);
    }));
} else {
    spawned.push(child_process.spawn(
        'node', 
        ['/home/james/libultragdb/proxy/proxy.js', '/dev/ttyUSB0', '8080'], 
        {
            detached: true,
        }
    ));
}

Promise.all(waitFor).then(() => {
    fs.writeFileSync('./launch/last_launch.txt', spawned.map(child => child.pid.toString()).join(','));
    process.exit(0);
});