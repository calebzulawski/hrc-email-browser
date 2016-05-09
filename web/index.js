var sqlite3 = require('sqlite3').verbose();
var express = require('express');
var app = express();

var db = new sqlite3.Database('../hrcemail.sqlite', sqlite3.OPEN_READONLY);

app.get('/', function (req, res) {
    res.send('Hello World!');
});

app.get('/email/:id', function (req, res, next) {

});

app.param('id', function (req, res, next, id) {
    console.log('request for email: ' + id);
    db.serialize(function() {
        db.all("SELECT * from document WHERE docID LIKE ?", id, function(error, rows) {
            if (error != null) {
                console.log(error);
                res.send('sqlite3 error!');
                return;
            }
            if (rows.length == 0) {
                res.send('Could not find email with id: ' + id);
                return;
            }
            console.log(rows);
            res.send('Found email with id: ' + id);
        });
    });
});

app.listen(3000, function () {
    console.log('hrc-email-browser server running on port 3000!');
});
