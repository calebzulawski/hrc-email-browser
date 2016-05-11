#!/bin/env python3
import sqlite3
import gensim

dictionary = gensim.corpora.Dictionary()
corpus = []
docIDs = []

print('Building corpus...')

con = sqlite3.connect('../hrcemail.sqlite')
cur = con.cursor()
cur.execute('select docID, docText from document')
while True:
    result = cur.fetchone()
    if not result:
        break
    docIDs.append(result[0])
    doc = result[1].lower().split()
    dictionary.add_documents([doc])
    corpus.append(dictionary.doc2bow(doc))

print('Training LDA...')

lda = gensim.models.ldamodel.LdaModel(corpus, num_topics=10, minimum_probability=0, iterations=1000)

print('Adding topic distributions to SQLite database...')

cur.execute('drop table if exists topics')

cur.execute('create table topics (docID, T0, T1, T2, T3, T4, T5, T6, T7, T8, T9)')

for docID, docBow in zip(docIDs, corpus):
    data = [docID] + [x[1] for x in lda[docBow]]
    cur.execute('insert into topics values (?,?,?,?,?,?,?,?,?,?,?)', data)

con.commit()
con.close()
