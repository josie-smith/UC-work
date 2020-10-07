const db = require('../../config/db');
const passwords = require('../middleware/passwords.js')
const fs = require('mz/fs');

const photoDirectory = './storage/photos/';

function isInt(value) {
    return !isNaN(value) &&
        parseInt(Number(value)) == value &&
        !isNaN(parseInt(value, 10));
}

exports.getAll = async function ( q, categoryId, authorId, sortBy ) {
    const pool = db.getPool();
    let query = 'SELECT P.petition_id as petitionId, title, C.name as category, U.name as authorName, S.signatures as signatureCount';
    query += ' FROM Petition P LEFT OUTER JOIN';
    query += ' (SELECT petition_id, count(petition_id) as signatures FROM Signature GROUP BY petition_id) S';
    query += ' ON P.petition_id = S.petition_id';
    query += ' LEFT OUTER JOIN';
    query += ' User U';
    query += ' ON P.author_id = U.user_id';
    query += ' LEFT OUTER JOIN';
    query += ' Category C';
    query += ' ON P.category_id = C.category_id';
    let where = ' WHERE';
    //filters
    if (categoryId) {
        try {
            let cat_query = 'select category_id from Category';
            var [cat_ids] = await pool.query(cat_query);
        } catch (err) {
            console.log(err);
            throw ([500, 'Internal Server Error: ' + err]);
        }
        if ( cat_ids.map(cat => cat.category_id).includes(parseInt(categoryId)) ) {
            query += where + ' P.category_id = ' + categoryId;
            where = ' AND';
        } else {
            throw ([400, 'Bad Request: invalid category ID']);
        }
    }
    if (authorId) {
        if ( isInt(authorId) ) {
            query += where + ' author_id = ' + authorId;
            where = ' AND';
        } else {
            throw ([400, 'Bad Request: data.authorId should be an integer']);
        }
    }
    //search
    if (q) {
        query += where + " title like '%" + q + "%'";
    }
    //sort
    if (!sortBy || sortBy == "SIGNATURES_DESC" ) {
        query += ' order by signatures desc';
    } else if (sortBy == "ALPHABETICAL_ASC") {
        query += ' order by title asc';
    } else if (sortBy == "ALPHABETICAL_DESC") {
        query += ' order by title desc';
    } else if (sortBy == "SIGNATURES_ASC") {
        query += ' order by signatures asc';
    } else {
        throw ([400, 'Bad Request: data.sortBy should be equal to one of the allowed values']);
    }

    try {
        const [ rows ] = await pool.query( query );
        return rows;
    } catch (err) {
        console.log(err);
        throw ([500, 'Internal Server Error: ' + err]);
    }
};

exports.insert = async function( auth, title, description, categoryId, closingDate ) {
    const authorised = await passwords.authorise(null, auth);
    const pool = db.getPool();
    if (authorised.code == 200) {
        const now = new Date();
        if (closingDate) {
            closingDate = new Date(closingDate);
        } else {
            closingDate = null;
        }
        if (!closingDate || closingDate > now) {
            try {
                var query = 'select category_id from Category';
                var [cat_ids] = await pool.query(query);
            } catch (err) {
                console.log(err);
                throw ([500, 'Internal Server Error: ' + err]);
            }
            if ( cat_ids.map(cat => cat.category_id).includes(parseInt(categoryId)) ) {
                try {
                    query = 'insert into Petition (title, description, category_id, closing_date, author_id, created_date) values ( ?, ?, ?, ?, ?, ? )';
                    var [rows] = await pool.query(query, [title, description, categoryId, closingDate, authorised.id, now]);
                } catch (err) {
                    console.log(err);
                    throw ([500, 'Internal Server Error: ' + err]);
                }
                return rows.insertId;
            } else {
                throw ([400, "Bad request: categoryId does not match any existing category"]);
            }
        } else {
            throw ([400, 'Bad request: closingDate must be in the future']);
        }
    } else if (authorised.code == 401) {
        throw ([401, 'Unauthorized']);
    } else if (authorised.code == 404) {
        throw ([404, 'Not found']);
    } else {
        throw ([403, 'Forbidden']);
    }
};

exports.getOne = async function ( id ) {
    const pool = db.getPool();
        try {
            var query = 'select petition_id, author_id from Petition where petition_id = ( ? )';
            var [rows] = await pool.query(query, [id]);
        } catch (err) {
            console.log(err);
            throw ([500, 'Internal Server Error: ' + err]);
        }
        if (rows.length > 0) {
            try {
                query = 'SELECT P.petition_id as petitionId, title, description, P.author_id as authorId, A.name as authorName, A.city as authorCity, A.country as authorCountry, S.signatures as signatureCount, C.name as category, P.created_date as createdDate, P.closing_date as closingDate';
                query += ' FROM Petition P LEFT OUTER JOIN';
                query += ' (SELECT petition_id, count(petition_id) as signatures FROM Signature GROUP BY petition_id) S';
                query += ' ON P.petition_id = S.petition_id';
                query += ' LEFT OUTER JOIN';
                query += ' User A';
                query += ' ON P.author_id = A.user_id';
                query += ' LEFT OUTER JOIN';
                query += ' Category C';
                query += ' ON P.category_id = C.category_id';
                query += ' where P.petition_id = ( ? )';
                const [ rows ] = await pool.query( query, [id] );
                return rows;
            } catch (err) {
                console.log(err);
                throw ([500, 'Internal Server Error: ' + err]);
            }
        } else {
            throw ([404, 'Not found']);
        }
};

exports.update = async function( auth, id, title, description, categoryId, closingDate ) {
    const authorised = await passwords.authorise(null, auth);
    const pool = db.getPool();
    if (authorised.code == 200) {
        try {
            var query = 'select petition_id, author_id from Petition where petition_id = ( ? )';
            var [rows] = await pool.query(query, [id]);
        } catch (err) {
            console.log(err);
            throw ([500, 'Internal Server Error: ' + err]);
        }
        if (rows.length > 0) {
            // this petition exists
            if (rows[0].author_id == authorised.id) {
                // petition was by this user
                if (closingDate) {
                    const now = new Date();
                    if (closingDate > now) {
                        try {
                            query = 'update Petition set closing_date = ( ? )';
                            await pool.query(query, [closingDate]);
                        } catch (err) {
                            console.log(err);
                            throw ([500, 'Internal Server Error: ' + err]);
                        }
                    } else {
                        throw ([400, 'Bad request: closing date must be in the future']);
                    }
                }

                if (categoryId) {
                    try {
                        query = 'select category_id from Category';
                        var [cat_ids] = await pool.query(query);
                    } catch (err) {
                        console.log(err);
                        throw ([500, 'Internal Server Error: ' + err]);
                    }
                    if (cat_ids.map(cat => cat.category_id).includes(parseInt(categoryId))) {
                        try {
                            query = 'update Petition set category_id = ( ? )';
                            await pool.query(query, [categoryId]);
                        } catch (err) {
                            console.log(err);
                            throw ([500, 'Internal Server Error: ' + err]);
                        }
                    } else {
                        throw ([400, "Bad request: categoryId doesn't exist"]);
                    }
                }

                if (title) {
                    try {
                        query = 'update Petition set title = ( ? )';
                        await pool.query(query, [title]);
                    } catch (err) {
                        console.log(err);
                        throw ([500, 'Internal Server Error: ' + err]);
                    }
                }

                if (description) {
                    try {
                        query = 'update Petition set description = ( ? )';
                        await pool.query(query, [description]);
                    } catch (err) {
                        console.log(err);
                        throw ([500, 'Internal Server Error: ' + err]);
                    }
                }

                if (!closingDate && !categoryId && !title && !description) {
                    throw ([400, "Bad request: no valid fields provided"]);
                }
            } else {
                throw ([403, 'Forbidden']);
            }
        } else {
            throw ([404, 'Not found']);
        }
    } else if (authorised.code == 401) {
        throw ([401, 'Unauthorized']);
    } else if (authorised.code == 404) {
        throw ([404, 'Not found']);
    } else {
        throw ([403, 'Forbidden']);
    }
};

exports.remove = async function( auth, id ) {
    const authorised = await passwords.authorise(null, auth);
    const pool = db.getPool();
    if (authorised.code == 200) {
        try {
            var query = 'select petition_id, author_id from Petition where petition_id = ( ? )';
            var [rows] = await pool.query(query, [id]);
        } catch (err) {
            console.log(err);
            throw ([500, 'Internal Server Error: ' + err]);
        }
        if (rows.length > 0) {
            //petition exists
            if (rows[0].author_id == authorised.id) {
                //petition is  by this user
                try {
                    let query = 'delete from Petition where petition_id = ( ? )';
                    await pool.query( query, [ id ] );
                    query = 'delete from Signature where petition_id = ( ? )';
                    await pool.query( query, [ id ] );
                } catch (err) {
                    console.log(err);
                    throw ([500, 'Internal Server Error: ' + err]);
                }
            } else {
                throw ([403, 'Forbidden']);
            }
        } else {
            throw ([404, 'Not found']);
        }
    } else if (authorised.code == 401) {
        throw ([401, 'Unauthorized']);
    } else if (authorised.code == 404) {
        throw ([404, 'Not found']);
    } else {
        throw ([403, 'Forbidden']);
    }
};

exports.getCategories = async function () {
    const pool = db.getPool();
    try {
        const query = 'SELECT category_id as categoryId, name FROM Category';
        const [ rows ] = await pool.query( query );
        return rows;
    } catch (err) {
        console.log(err);
        throw ([500, 'Internal Server Error: ' + err]);
    }
};

exports.getPhoto = async function (id) {
    const pool = db.getPool();
    try {
        const query = 'select photo_filename from Petition where petition_id = ( ? )';
        var [ rows ] = await pool.query( query, [id]);
    } catch (err) {
        console.log(err);
        throw ([500, 'Internal Server Error: ' + err]);
    }
    if (rows.length == 0) {
        throw ([404, 'Not found']);
    } else {
        return rows[0].photo_filename;
    }
};

exports.setPhoto = async function (id, auth, photo, fileExt) {
    const authorised = await passwords.authorise(null, auth);
    const pool = db.getPool();
    if (authorised.code == 200) {
        try {
            var query = 'select petition_id, author_id from Petition where petition_id = ( ? )';
            var [rows] = await pool.query(query, [id]);
        } catch (err) {
            console.log(err);
            throw ([500, 'Internal Server Error: ' + err]);
        }
        if (rows.length > 0) {
            // petition exists
            if (rows[0].author_id == authorised.id) {
                // author is the user
                var filename = 'petition_' + id + fileExt;
                // photo.pipe(fs.createWriteStream(photoDirectory + filename));
                fs.writeFile(photoDirectory + filename, photo, "binary", function(err) {
                    if(err) {
                        console.log(err);
                        throw ([500, 'Internal Server Error: ' + err]);
                    }
                });

                //update the database
                try {
                    query = 'select photo_filename from Petition where petition_id = ( ? )';
                    const [rows] = await pool.query(query, [id]);

                    //check if photo already exists
                    let result = 201;
                    if (rows.length == 0 || rows[0].photo_filename != null) {
                        result = 200;
                    }

                    //update the photo
                    query = 'update Petition set photo_filename = ( ? ) where petition_id = ( ? )';
                    await pool.query(query, [filename, id]);
                    return result;
                } catch (err) {
                    console.log(err);
                    throw ([500, 'Internal Server Error: ' + err]);
                }
            } else {
                throw ([403, 'Forbidden']);
            }
        } else {
            throw ([404, 'Not found']);
        }
    } else if (authorised.code == 401) {
        throw ([401, 'Unauthorized']);
    } else if (authorised.code == 404) {
        throw ([404, 'Not found']);
    } else {
        throw ([403, 'Forbidden']);
    }
};

exports.getSignatures = async function (id) {
    const pool = db.getPool();
    try {
        var query = 'select petition_id from Petition';
        var [petition_ids] = await pool.query(query, [id]);
    } catch (err) {
        console.log(err);
        throw ([500, 'Internal Server Error: ' + err]);
    }
    if ( petition_ids.map(pet => pet.petition_id).includes(parseInt(id)) ) {
        // petition exists
        try {
            query = 'select S.signatory_id as signatoryId, U.name as name, U.city as city, U.country as country, S.signed_date as signedDate';
            query += ' FROM Signature S LEFT OUTER JOIN User U ON S.signatory_id = U.user_id';
            query += ' where S.petition_id = ( ? ) order by S.signed_date asc';
            const [rows] = await pool.query(query, [id]);
            return rows;
        } catch (err) {
            console.log(err);
            throw ([500, 'Internal Server Error: ' + err]);
        }
    } else {
        throw ([404, 'Not found']);
    }
};

exports.sign = async function( id, auth ) {
    const authorised = await passwords.authorise(null, auth);
    const pool = db.getPool();
    if (authorised.code == 200) {
        //user is signed in
        try {
            var query = 'select petition_id from Petition';
            var [petition_ids] = await pool.query(query, [id]);
        } catch (err) {
            console.log(err);
            throw ([500, 'Internal Server Error: ' + err]);
        }
        if ( petition_ids.map(pet => pet.petition_id).includes(parseInt(id)) ) {
            // petition_id exists
            try {
                //check if they have already signed the petition
                let query = 'select petition_id from Signature where signatory_id = ( ? )';
                var [signed_petitions] = await pool.query( query, [ authorised.id ] );
            } catch (err) {
                console.log(err);
                throw ([500, 'Internal Server Error: ' + err]);
            }
            if (!signed_petitions.map(pet => pet.petition_id).includes(parseInt(id))) {
                //check if the petition has closed
                try {
                    let query = 'select closing_date from Petition where petition_id = ( ? )';
                    var [closing_date] = await pool.query( query, [ id ] );
                } catch (err) {
                    console.log(err);
                    throw ([500, 'Internal Server Error: ' + err]);
                }
                const now = new Date();
                if (!closing_date[0].closing_date || closing_date[0].closing_date > now) {
                    try {
                        query = 'insert into Signature (petition_id, signatory_id, signed_date) values ( ?, ?, ? )';
                        await pool.query(query, [id, authorised.id, now]);
                    } catch (err) {
                        console.log(err);
                        throw ([500, 'Internal Server Error: ' + err]);
                    }
                } else {
                    throw ([403, 'Forbidden: cannot sign a petition that has closed']);
                }
            } else {
                throw ([403, 'Forbidden: cannot sign a petition more than once']);
            }
        } else {
            throw ([404, 'Not found']);
        }
    } else if (authorised.code == 401) {
        throw ([401, 'Unauthorized']);
    } else if (authorised.code == 404) {
        throw ([404, 'Not found']);
    } else {
        throw ([403, 'Forbidden']);
    }
};

exports.unsign = async function( id, auth ) {
    const authorised = await passwords.authorise(null, auth);
    const pool = db.getPool();
    // check if petition exists
    try {
        let query = 'select petition_id from Petition where petition_id = ( ? )';
        var [petition] = await pool.query(query, [id]);
        console.log(petition);
    } catch (err) {
        console.log(err);
        throw ([500, 'Internal Server Error: ' + err]);
    }
    if (authorised.code == 200) {
        if (petition.length > 0) {
            //user is signed in
            try {
                //check if they have already signed the petition
                let query = 'select petition_id from Signature where signatory_id = ( ? ) and petition_id = ( ? )';
                var [signed_petition] = await pool.query(query, [authorised.id, id]);
            } catch (err) {
                console.log(err);
                throw ([500, 'Internal Server Error: ' + err]);
            }
            if (signed_petition.length != 0) {
                try {
                    // check if it is after the closing date
                    let query = 'select closing_date, author_id from Petition where petition_id = ( ? )';
                    var [rows] = await pool.query(query, [id]);
                } catch (err) {
                    console.log(err);
                    throw ([500, 'Internal Server Error: ' + err]);
                }
                const now = new Date();
                if (!rows[0].closing_date || rows[0].closing_date > now) {
                    if (rows[0].author_id != authorised.id) {
                        try {
                            query = 'delete from Signature where petition_id = ( ? ) and signatory_id = ( ? )';
                            await pool.query(query, [id, authorised.id]);
                        } catch (err) {
                            console.log(err);
                            throw ([500, 'Internal Server Error: ' + err]);
                        }
                    } else {
                        throw ([403, 'Forbidden: cannot remove signature from a petition you created']);
                    }
                } else {
                    throw ([403, 'Forbidden: cannot remove signature from a petition that is closed']);
                }
            } else {
                throw ([403, 'Forbidden: cannot remove signature from a petition without first signing it']);
            }
        } else {
            throw ([404, 'Forbidden']);
        }
    } else if (authorised.code == 401) {
        throw ([401, 'Unauthorized']);
    } else if (authorised.code == 404) {
        throw ([404, 'Not found']);
    } else {
        throw ([403, 'Forbidden']);
    }
};