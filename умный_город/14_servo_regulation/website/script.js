let server_ip = '{{ context.ip }}', web_port = {{ context.web_port }};

async function init(url) {
    let response = await fetch(url, {
        method: 'GET'
    });
    if (response.status == 200) {
        let js = JSON.parse(await response.text());
        let table = document.getElementById('users');
        table.innerHTML = '<tr><th>ID</th><th>Clicks</th></tr>';
        for (let i = 0; i < js.length; i++) {
            let row = table.insertRow();
            row.innerHTML = `<td>${js[i]['ID']}</td><td>${js[i]['Clicks']}</td>`;
        }
    } else {
        await init(url);
    }
}

async function update(url) {
    let response = await fetch(url, {
        method: 'GET'
    });
    if (response.status == 200) {
        let js = JSON.parse(await response.text());
        let table = document.getElementById('users');
        table.innerHTML = '<tr><th>ID</th><th>Clicks</th></tr>';
        for (let i = 0; i < js.length; i++) {
            let row = table.insertRow();
            row.innerHTML = `<td>${js[i]['ID']}</td><td>${js[i]['Clicks']}</td>`;
        }
    }
    await update(url);
}

window.onload = function() {
    init(`http://${server_ip}:${web_port}/init`);
    update(`http://${server_ip}:${web_port}/update`);
}