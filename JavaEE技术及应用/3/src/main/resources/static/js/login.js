document.addEventListener("DOMContentLoaded", () => {
    const form = document.getElementById("loginForm");
    let loginAttempts = 0; // 记录失败次数

    if (form) {
        // 页面加载时，自动填充已保存的用户名
        const savedUser = localStorage.getItem("username");
        if (savedUser) {
            document.getElementById("username").value = savedUser;
        }

        form.addEventListener("submit", async (event) => {
            event.preventDefault();

            const username = document.getElementById("username");
            const password = document.getElementById("password");
            let valid = true;

            username.classList.remove("error");
            password.classList.remove("error");

            if (username.value.trim() === "") {
                alert("用户名不能为空！");
                username.classList.add("error");
                valid = false;
            }

            if (password.value.trim() === "") {
                alert("密码不能为空！");
                password.classList.add("error");
                valid = false;
            }

            if (valid) {
                try {
                    const result = await login(username.value, password.value);

                    if (result.success) {
                        alert("登录成功！");
                        // 记住用户名
                        localStorage.setItem("username", username.value);
                        window.location.href = "index.html";
                    } else {
                        loginAttempts++;
                        alert(`登录失败！${result.message} 剩余尝试次数：${3 - loginAttempts}`);
                        password.classList.add("error");
                        password.value = "";

                        if (loginAttempts >= 3) {
                            alert("错误次数过多，请稍后再试！");
                            form.querySelector("button").disabled = true;
                        }
                    }
                } catch (error) {
                    console.error('登录请求失败:', error);
                    alert("登录请求失败，请检查网络连接");
                }
            }
        });
    }
});