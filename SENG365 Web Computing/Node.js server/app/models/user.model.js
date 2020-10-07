const fs = require('mz/fs');
const db = require('../../config/db');
const passwords = require('../middleware/passwords.js')

const photoDirectory = './storage/photos/';

function validateEmail(email) {
    var re = /^(([^<>()\[\]\\.,;:\s@"]+(\.[^<>()\[\]\\.,;:\s@"]+)*)|(".+"))@((\[[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\])|(([a-zA-Z\-0-9]+\.)+[a-zA-Z]{2,}))$/;
    return re.test(String(email).toLowerCase());
}


exports.register = async function (name, email, password, city, country) {
    const pool = db.getPool();
    //validate email
    if (validateEmail(email)) {
        //check duplicate email
        try {
            const checkDuplicate = 'select * from User where email = ( ? )';
            var [ checkRows ] = await pool.query( checkDuplicate, [ email ] );
        } catch (err) {
            console.log(err);
            throw ([500, 'Internal Server Error: ' + err]);
        }
        if (checkRows.length == 0) {

            //check password is not empty
            if (password) {
                try {
                    password = await passwords.hash(password);
                    const query = 'insert into User (name, email, password, city, country) values ( ?, ?, ?, ?, ? )';
                    const [ rows ] = await pool.query( query, [name, email, password, city, country] );
                    return rows.insertId;
                } catch (err) {
                    console.log(err);
                    throw ([500, 'Internal Server Error: ' + err]);
                }
            } else {
                throw ([400, 'Bad Request: data.password should NOT be shorter than 1 characters']);
            }
        } else {
            throw ([400, 'Bad Request: email already in use']);
        }
    } else {
        throw ([400, 'Bad Request: data.email should match format "email"']);
    }
};

exports.login = async function (email, password) {
    const pool = db.getPool();
    // get the password for the current email
    try {
        const hashquery = 'select user_id, password from User where email = ( ? )';
        var [ rows ] = await pool.query( hashquery, [email] );
    } catch (err) {
        console.log(err);
        throw ([500, 'Internal Server Error: ' + err]);
    }
    // if the email exists
    if (rows.length > 0) {
        const hash = rows[0].password;
        const user_id = rows[0].user_id;
        // if the password is correct
        if (passwords.compare(password, hash)) {
            //generate auth token
            try {
                const auth = passwords.auth();
                const query = 'update User set auth_token  = ( ? ) where email = ( ? )';
                await pool.query( query, [auth, email] );
                return [user_id, auth];
            } catch (err) {
                console.log(err);
                throw ([500, 'Internal Server Error: ' + err]);
            }
        } else {
            throw ([400, 'Bad Request: invalid email/password supplied']);
        }
    } else {
        throw ([400, 'Bad Request: invalid email/password supplied']);
    }
};

exports.logout = async function (auth) {
    const pool = db.getPool();
    if (auth != null) {
        try {
            let query = 'select user_id from User where auth_token  = ( ? )';
            var [user_id] = await pool.query(query, [auth]);
        } catch (err) {
            console.log(err);
            throw ([500, 'Internal Server Error: ' + err]);
        }
        if (user_id.length != 0) {
            try {
                let query = 'update User set auth_token = NULL where auth_token = ( ? )';
                await pool.query(query, [auth]);
                return;
            } catch (err) {
                console.log(err);
                throw ([500, 'Internal Server Error: ' + err]);
            }
        } else {
            throw ([401, 'Unauthorized']);
        }
    } else {
        throw ([401, 'Unauthorized']);
    }
};

exports.get = async function ( id, auth ) {
    const authorised = await passwords.authorise(id, auth);
    const pool = db.getPool();
    try {
        let [ rows ] = [];
        if (authorised.code == 200) {
            const query = 'select name, city, country, email from User where user_id = ( ? )';
            [ rows ] = await pool.query( query, [id] );
        } else {
            const query = 'select name, city, country from User where user_id = ( ? )';
            [ rows ] = await pool.query( query, [id] );
        }
        return rows[0];
    } catch (err) {
        console.log(err);
        throw ([500, 'Internal Server Error: ' + err]);
    }
};

exports.update = async function( id, auth, name, email, password, currentPassword, city, country ) {
    const authorised = await passwords.authorise(id, auth);
    const pool = db.getPool();
    if (authorised.code == 200) {
        //check password is not empty
        if (password || currentPassword) {
            try {
                var query = 'select password from User where user_id = ( ? )';
                var [ rows ] = await pool.query( query, [id] );
                var hash = rows[0].password;
            } catch (err) {
                console.log(err);
                throw ([500, 'Internal Server Error: ' + err]);
            }
            if (password) {
                if (!passwords.compare(password, hash)) {
                    if (currentPassword) {
                        // if setting a new password
                        if (passwords.compare(currentPassword, hash)) {
                            // var correctPassword = true;
                            try {
                                password = await passwords.hash(password);
                                query = 'update User set password = ( ? ) where user_id = ( ? )';
                                await pool.query( query, [password, id] );
                            } catch (err) {
                                console.log(err);
                                throw ([500, 'Internal Server Error: ' + err]);
                            }
                        } else {
                            throw ([401, 'Unauthorized: data.currentPassword is incorrect']);
                        }
                    } else {
                        throw ([400, 'Bad Request: incorrect password']);
                    }
                }
            } else {
                throw ([400, "Bad Request: password should NOT be shorter than 1 characters"]);
            }
        }

        if (email) {
            try {
                const checkDuplicate = 'select * from User where email = ( ? )';
                var [ checkRows ] = await pool.query( checkDuplicate, [ email ] );
            } catch (err) {
                console.log(err);
                throw ([500, 'Internal Server Error: ' + err]);
            }

            if (validateEmail(email)) {
                if (checkRows.length == 0) {
                    try {
                        query = 'update User set email = ( ? ) where user_id = ( ? )';
                        await pool.query( query, [email, id] );
                    } catch (err) {
                        console.log(err);
                        throw ([500, 'Internal Server Error: ' + err]);
                    }
                } else {
                    throw ([400, 'Bad Request: email already in use']);
                }
            } else {
                throw ([400, 'Bad Request: data.email should match format "email"']);
            }
        }

        if (name) {
            try {
                query = 'update User set name = ( ? ) where user_id = ( ? )';
                await pool.query( query, [name, id] );
            } catch (err) {
                console.log(err);
                throw ([500, 'Internal Server Error: ' + err]);
            }
        }

        if (city) {
            try {
                query = 'update User set city = ( ? ) where user_id = ( ? )';
                await pool.query( query, [city, id] );
            } catch (err) {
                console.log(err);
                throw ([500, 'Internal Server Error: ' + err]);
            }
        }

        if (country) {
            try {
                query = 'update User set country = ( ? ) where user_id = ( ? )';
                await pool.query( query, [country, id] );
            } catch (err) {
                console.log(err);
                throw ([500, 'Internal Server Error: ' + err]);
            }
        }

        if (!name && !email && !password && !currentPassword && !city && !country) {
            throw ([400, 'Bad Request: you must provide some details to update']);
        }
        return;
    } else if (authorised.code == 401) {
        throw ([401, 'Unauthorized']);
    } else if (authorised.code == 404) {
        throw ([404, 'Not found']);
    } else {
        throw ([403, 'Forbidden']);
    }
};

exports.getPhoto = async function ( id ) {
    const pool = db.getPool();
    try {
        const query = 'select photo_filename from User where user_id = ( ? )';
        var [ rows ] = await pool.query( query, [id] );
    } catch (err) {
        console.log(err);
        throw ([500, 'Internal Server Error: ' + err]);
    }
    if (rows.length == 0 || rows[0].photo_filename == null) {
        throw ([404, 'Not found']);
    }
    return rows[0].photo_filename;
};

exports.setPhoto = async function( id, auth, photo, fileExt ) {
    const authorised = await passwords.authorise(id, auth);
    const pool = db.getPool();
    if (authorised.code == 200) {
        //save photo
        var filename = 'user_' + id + fileExt;
        // photo.pipe(fs.createWriteStream(photoDirectory + filename));
        fs.writeFile(photoDirectory + filename, photo, "binary", function(err) {
            if(err) {
                console.log(err);
                throw ([500, 'Internal Server Error: ' + err]);
            }
        });

        //update the database
        try {
            query = 'select photo_filename from User where user_id = ( ? )';
            const [ rows ] = await pool.query( query, [id] );

            //check if photo already exists
            let result = 200;
            if (rows.length == 0 || rows[0].photo_filename == null) {
                result = 201;
            }

            //update the photo
            query = 'update User set photo_filename = ( ? ) where user_id = ( ? )';
            await pool.query( query, [filename, id] );
            return result;
        } catch (err) {
            console.log(err);
            throw ([500, 'Internal Server Error: ' + err]);
        }
    } else if (authorised.code == 401) {
        throw ([401, 'Unauthorized']);
    } else if (authorised.code == 404) {
        throw ([404, 'Not found']);
    } else {
        throw ([403, 'Forbidden']);
    }
};

exports.removePhoto = async function( id, auth ) {
    const authorised = await passwords.authorise(id, auth);
    const pool = db.getPool();
    if (authorised.code == 200) {
        //update the database
        try {
            //check if there is a photo
            query = 'select photo_filename from User where user_id = ( ? )';
            var [ rows ] = await pool.query( query, [id] );
        } catch (err) {
            console.log(err);
            throw ([500, 'Internal Server Error: ' + err]);
        }
        const filename = rows[0].photo_filename;
        if (filename == null) {
            throw ([404, 'Not found']);
        }
        try {
            fs.unlinkSync(photoDirectory + filename);
            query = 'update User set photo_filename = NULL where user_id = ( ? )';
            await pool.query( query, [id] );
            return;
        } catch (err) {
            console.log(err);
            throw ([500, 'Internal Server Error: ' + err]);
        }
    } else if (authorised.code == 401) {
        throw ([401, 'Unauthorized']);
    } else if (authorised.code == 404) {
        throw ([404, 'Not found']);
    } else {
        throw ([403, 'Forbidden']);
    }
};
