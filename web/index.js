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
        emailRow = db.all("SELECT * FROM document WHERE docID LIKE ?", id, function(error, emailRow) {
            if (error != null) {
                console.log(error);
                res.send('sqlite3 error!');
                return;
            }
            if (emailRow.length == 0) {
                res.send('Could not find email with id: ' + id);
                return;
            }
            db.all("SELECT * FROM topics WHERE docID LIKE ?", id, function(error, thisTopic) {
                console.log(error);
                console.log(thisTopic);
                db.all("SELECT docID, \
                        (T0 - $t0)*(T0 - $t0) + \
                        (T1 - $t1)*(T1 - $t1) + \
                        (T2 - $t2)*(T2 - $t2) + \
                        (T3 - $t3)*(T3 - $t3) + \
                        (T4 - $t4)*(T4 - $t4) + \
                        (T5 - $t5)*(T5 - $t5) + \
                        (T6 - $t6)*(T6 - $t6) + \
                        (T7 - $t7)*(T7 - $t7) + \
                        (T8 - $t8)*(T8 - $t8) + \
                        (T9 - $t9)*(T9 - $t9) AS distance \
                        FROM topics \
                        WHERE docID NOT LIKE $id \
                        ORDER BY distance ASC", {
                            $t0: thisTopic[0].T0,
                            $t1: thisTopic[0].T1,
                            $t2: thisTopic[0].T2,
                            $t3: thisTopic[0].T3,
                            $t4: thisTopic[0].T4,
                            $t5: thisTopic[0].T5,
                            $t6: thisTopic[0].T6,
                            $t7: thisTopic[0].T7,
                            $t8: thisTopic[0].T8,
                            $t9: thisTopic[0].T9,
                            $id: id
                        }, function(error, distances) {
                    res.render('email', {
                        id:      emailRow[0].docID,
                        subject: emailRow[0].subject,
                        date:    emailRow[0].docDate,
                        from:    emailRow[0].from,
                        to:      emailRow[0].to,
                        body:    emailRow[0].docText,
                        close:   distances.slice(0,10),
                        far:     distances.slice(distances.length-10, distances.length+1).reverse()
                    });
                });
            })
        });
    });
});


// run the server
app.listen(3000, function () {
    console.log('hrc-email-browser server running on port 3000!');
});
