async function init(url) {
    let response = await fetch(url, {
        method: 'GET'
    });
    if (response.status == 200) {
        let field = document.getElementById("info");
        field.innerHTML = await response.text();
    } else {
        await init(url);
    }
}

async function update(url) {
    let response = await fetch(url, {
        method: 'GET'
    });
    if (response.status == 200) {
        let field = document.getElementById("info");
        field.innerHTML = await response.text();
    }
    await update(url);
}

init('http://192.168.0.20:7001/init');
update('http://192.168.0.20:7001/update');