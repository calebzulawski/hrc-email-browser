var sqlite3 = require('sqlite3').verbose();
var express = require('express');
var swig    = require('swig');

var app = express();
var db = new sqlite3.Database( __dirname + '/../hrcemail.sqlite', sqlite3.OPEN_READONLY);

// Set up static files
app.use('/static', express.static('public'));

// Set up views
app.engine('html', swig.renderFile);
app.set('view engine', 'html');
app.set('views', __dirname + '/views');
app.set('view cache', false);
swig.setDefaults({ cache: false });

// Set up paths:
//
// root/
//   Main page
//
// root/search/:query
//   Search database for :query
//
// root/email/:id
//   Display email with database ID :id

app.get('/', function (req, res) {
    res.send('Hello World!');
});

// this is super unsafe but database is loaded read-only...
function buildSearch(query) {
    query = decodeURI(query).split(" ");
    columns = "";
    conditions = "";
    for (var i = 0; i < query.length; i++) {
        if (i != 0) {
            columns += ",";
            conditions += "OR ";
        }
        columns += "(length(docText) - length(replace(upper(docText), \"" + query[i].toUpperCase() + "\", \'\')))/length(\"" + query[i] + "\") AS R" + i;
        conditions += "docText LIKE \"%" + query[i] + "%\"";
    }
    return {query: query, sql: "SELECT docID, " + columns + " FROM document WHERE " + conditions + ";"};
}

app.get('/search/:search', function (req, res, next) {});
app.get('/email/:email', function (req, res, next) {});

app.param('search', function (req, res, next, query) {
    console.log('query: ' + query);
    db.serialize(function() {
        search = buildSearch(query);
        console.log(search.sql)
        db.all(search.sql, function(error, rows) {
            if (error != null) {
                console.log(error);
                res.send('sqlite3 error!');
                return;
            }
            console.log(rows);
            res.send('searched');
        });
    });
});

app.param('email', function (req, res, next, id) {
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
            res.render('email', {
                id:      rows[0].docID,
                subject: rows[0].subject,
                date:    rows[0].docDate,
                from:    rows[0].from,
                to:      rows[0].to,
                body:    rows[0].docText
            });
        });
    });
});


// run the server
app.listen(3000, function () {
    console.log('hrc-email-browser server running on port 3000!');
});
