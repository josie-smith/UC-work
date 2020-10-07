const bcrypt = require('bcrypt');
const crypto = require('crypto');
const db = require('../../config/db');

exports.hash = function (plain) {
    const saltRounds = 1;
    return bcrypt.hash(plain, saltRounds);
}

exports.compare = function (plain, hash) {
    return bcrypt.compareSync(plain, hash);
}

exports.auth = function () {
    return crypto.randomBytes(16).toString('hex');
}

exports.authorise = async function (id, auth) {
    if (auth != null) {
        //validate auth
        try {
            var pool = db.getPool();
            var query = 'select user_id from User where auth_token  = ( ? )';
            var [ user_id ] = await pool.query( query, [auth] );
        } catch (err) {
            console.log(err.sql);
            throw (err);
        }
        if ( user_id.length > 0) {
            if (id) {
                try {
                    query = 'select * from User where user_id  = ( ? )';
                    var [ rows ] = await pool.query( query, [id] );
                } catch (err) {
                    console.log(err.sql);
                    throw (err);
                }
                if (rows.length > 0) {

                    if (user_id[0].user_id == id) {
                        return {"code": 200};
                    } else {
                        return {"code": 403}; //trying to access another user
                    }
                } else {
                    return {"code": 404}; // user id does not exist
                }
            } else {
                return {"code": 200, "id": user_id[0].user_id };
            }
        } else {
            return {"code": 401}; // auth token not valid
        }
    } else {
        return {"code": 401}; // no auth token given
    }
}
