let server_ip = '{{ context.ip }}', web_port = {{ context.web_port }};

let battery_state = {0: 'Discharged', 1: 'Charging', 2: 'Discharging', 3: 'Charged'};

function handle_data(response) {
    let js = JSON.parse(response);
    for (let i = 0; i < Math.floor(js['charge'] / 10); i++) {
        document.getElementsByClassName("segment")[i].style.visibility = "visible";
    }
    document.getElementById('charge').innerHTML = `${js['charge']}%`;
    document.getElementById('state_full').innerHTML = `<b>State:</b> ${battery_state[js['state']]}`;
    document.getElementById('state_short').innerHTML = `<b>State:</b> ${battery_state[js['state']]}`;
}

async function init(url) {
    let response = await fetch(url, {
        method: 'GET'
    });
    if (response.status == 200) {
        handle_data(await response.text());
    } else {
        await init(url);
    }
}

async function update(url) {
    let response = await fetch(url, {
        method: 'GET'
    });
    if (response.status == 200) {
        handle_data(await response.text());
    }
    await update(url);
}

window.onload = function() {
    init(`http://${server_ip}:${web_port}/init`);
    update(`http://${server_ip}:${web_port}/update`);
    document.getElementById('back_button').addEventListener('click', function() {
        window.location.href = `http://${server_ip}:${web_port}/`;
    });
}