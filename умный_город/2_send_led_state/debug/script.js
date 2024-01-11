function send_state(url, state) {
    fetch(url, {
        method: 'POST',
        body: state
    });
}

window.onload = function () {
    let verify = document.getElementById("verify");
    verify.onclick = function() {
        alert('hi');
        let state = document.getElementById("state");
        send_state('http://192.168.119.242:7000', state.checked ? '1' : '0');
    };
}