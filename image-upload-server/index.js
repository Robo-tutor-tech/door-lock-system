const express = require('express');
const multer = require('multer');
const path = require('path');
const data = require("./data.json")
const fs = require("node:fs");

const saveData = () => {
    fs.writeFileSync("./data.json", JSON.stringify(data, null, 2), 'utf-8')
}

const PORT = process.env.PORT || 3000;

const app = express();
app.use(express.static('uploads'))
app.use(express.text())

app.use((req, res, next) => {
    console.log(req.method, req.path, req.body)
    next();
})

const storage = multer.diskStorage({
    destination: './uploads/',
    filename: function (req, file, cb) {
        cb(null, file.fieldname + '-' + Date.now() + path.extname(file.originalname));
    }
});

const upload = multer({
    storage: storage,
    limits: {fileSize: 10 * 1024 * 1024}, // 10MB file size limit
    fileFilter: function (req, file, cb) {
        checkFileType(file, cb);
    }
}).single('image');

function checkFileType(file, cb) {
    const filetypes = /jpeg|jpg|png|gif/;
    const extname = filetypes.test(path.extname(file.originalname).toLowerCase());
    const mimetype = filetypes.test(file.mimetype);

    if (mimetype && extname) {
        return cb(null, true);
    } else {
        cb('Error: Images Only!');
    }
}

app.post('/upload', (req, res) => {
    upload(req, res, (err) => {
        if (err) {
            res.status(400).json({message: err});
        } else {
            if (req.file == undefined) {
                res.status(400).json({message: 'No file selected!'});
            } else {
                res.json({
                    message: 'Image uploaded successfully!',
                    file: `uploads/${req.file.filename}`
                });
            }
        }
    });
});

app.post('/generate-otp', (req, res) => {
    const id = req.body
    console.log("-------->", id, "<---------")
    if (data.validIds.includes(id.trim())) {
        data.otp = Math.floor(Math.random() * 10000).toString().padStart(4, '0');
        data.createdAt = new Date();
        data.status = ""
        data.createdBy = req.body.id
        saveData()
        res.send(`${data.mob}Your OTP is ${data.otp}, Generated by: ${id}, http://robotutortech.ddns.net:7456`)
    } else {
        res.status(400).send("INVALID ID");
    }
});

app.post("/verify-otp", (req, res) => {
    const otp = req.body
    console.log("-------->", otp, "<---------")
    const differenceInMinutes = (new Date() - data.createdAt) / (1000 * 60);
    if (differenceInMinutes <= 10) {
        if (otp === data.otp) {
            data.otp = ""
            data.createdAt = new Date().setFullYear(2000)
            saveData()
            res.send("Success")
        } else {
            res.status(400).send("Invalid otp")
        }
    } else {
        res.status(400).send("Expired otp")
    }
})
app.use('/uploads', express.static('uploads'));

app.put("/mob", (req, res) => {
    const mob = JSON.parse(req.body).mob
    console.log("-------->", mob, "<---------")
    if (isNaN(Number(mob)) || mob.toString().length !== 10) {
        res.status(400).send("Invalid mob")
    } else {
        data.mob = Number(mob)
        saveData()
        res.send("Success")
    }
})


app.get("/", (req, res) => {
    res.send(`
<!doctype html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>Door lock system</title>
</head>
<body>
<h1>Data</h1>
    <div>Mob: ${data.mob}
    <form id="form">
    <label>Enter mobile no.: </label>
    <input type="number" name="mob"/>
    <input type="submit" value="Update" />
</form>
</div>
<br />Ids: ${data.validIds.join("; ")}
<h1>Images</h1>
<div>

    <div style="display: flex; flex-wrap: wrap; gap: 16px;">${fs.readdirSync("./uploads", "UTF-8")
        .reverse()
        .slice(0, 20)
        .map(file => {
            return `<div>
                    <img src="/uploads/${file}" alt="${file}" width="520px" style="transform: rotate(180deg);"/>
                    <br/>
                    <span>${new Date(+file.slice(6, -4) + 5.5 * 60 * 60 * 1000).toLocaleString()}</span>
                    </div>`;
        })
        .join("<br />")
    }</div>
</div>
</body>
<script>
 document.getElementById("form").addEventListener("submit", function(event){
            event.preventDefault();
            const formData = new FormData(this)
            fetch('/mob', {method: "PUT", body: JSON.stringify({mob: formData.get('mob')})})
            window.location.reload()
        });
</script>
</html>
`)
})


app.listen(PORT, () => {
    console.log(`Server running on http://localhost:${PORT}`);
});
