#!/bin/env python3
import json
import sqlite3

JSON_FILE = "documents.json"
DB_FILE = "../hrcemail.sqlite"

documents = json.load(open(JSON_FILE))
conn = sqlite3.connect(DB_FILE)

c = conn.cursor()
c.execute('create table topics (docID, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9)')

for key, value in documents.items():
    data = [key] + value;
    c.execute('insert into topics values (?,?,?,?,?,?,?,?,?,?,?)', data)

conn.commit()
c.close()
