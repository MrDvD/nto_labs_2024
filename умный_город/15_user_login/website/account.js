let server_ip = '{{ context.ip }}', web_port = {{ context.web_port }};

function load_list() {
    for (let i = 0; i < 20; i++) {
        document.getElementById('list').innerHTML += '<tr><th>This</th><th>That</th></tr>';
    }
}

// async function login(user, pass) {
//     let url = `http://${server_ip}:${web_port}/login`;
//     let body = {'user': user, 'pass': pass}
//     console.log(body);
//     let response = await fetch(url, {
//         method: 'POST',
//         headers: new Headers({'content-type': 'application/json'}),
//         body: JSON.stringify(body)
//     });
//     if (response.status == 200) {
//         handle_login(await response.text());
//     }
// }

window.onload = function() {
    document.getElementById('back_button').addEventListener('click', function() {
        window.location.href = `http://${server_ip}:${web_port}/logout`;
    });
    load_list();
}