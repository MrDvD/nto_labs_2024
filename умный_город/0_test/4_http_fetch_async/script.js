function send_state(url, state) {
    fetch(url, {
        method: 'POST',
        body: state
    });
}

async function get_info(url) {
    const response = await fetch(url, {
        method: 'GET'
    });
    let field = document.getElementById("info");
    field.innerHTML = await response.text();
}

// иначе страница не загрузится => элемент не будет найден
window.onload = function () {
    // POST
    let verify = document.getElementById("verify");
    verify.onclick = function() {
        let state = document.getElementById("state");
        send_state('http://192.168.0.20:7001', state.checked ? '1' : '0');
    };

    // GET
    setInterval(get_info, 1000, 'http://192.168.0.20:7001');
}