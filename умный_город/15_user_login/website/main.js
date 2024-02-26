let server_ip = '{{ context.ip }}', web_port = {{ context.web_port }};

// function handle_login(response) {
//     switch (response) {
//         case '1':
//             let user = document.getElementsByClassName('field')[0].value;
//             let pass = document.getElementsByClassName('field')[1].value;
//             let form = document.createElement('form');
//             form.action = `http://${server_ip}:${web_port}/account`;
//             form.method = 'post';
//             // user
//             let input = document.createElement('input');
//             input.name = 'user';
//             input.value = user;
//             form.appendChild(input);
//             // pass
//             input = document.createElement('input');
//             input.name = 'pass';
//             input.value = pass;
//             form.appendChild(input);

//             document.body.appendChild(form); // перед отправкой формы её нужно вставить в документ
//             form.submit();
//             break;
//         case '0':
//             alert('User doesn\'t exist OR Incorrect password');
//             break;
//     }
// }

async function login(user, pass) {
    let url = `http://${server_ip}:${web_port}/login`;
    let body = {'user': user, 'pass': pass}
    console.log(body);
    let response = await fetch(url, {
        method: 'POST',
        headers: new Headers({'content-type': 'application/json'}),
        body: JSON.stringify(body)
    });
    if (response.status == 302) {
        location.href = `http://${server_ip}:${web_port}/account`;
    } else {
        alert(await response.text());
    }
}

window.onload = function() {
    document.getElementsByClassName('button')[1].addEventListener('click', function() {
        let user = document.getElementsByClassName('field')[0].value;
        let pass = document.getElementsByClassName('field')[1].value;
        login(user, pass);
    });
}