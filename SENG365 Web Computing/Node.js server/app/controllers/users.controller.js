const user = require('../models/user.model');
const fs = require('mz/fs');
const photoDirectory = './storage/photos/';

exports.register = async function (req, res) {
    const name = req.body.name;
    const email = req.body.email;
    const password = req.body.password;
    const city = req.body.city || null;
    const country = req.body.country || null;

    if (name) {
        if (email) {
            if (password) {
                try {
                    const user_id = await user.register(name, email, password, city, country);
                    res.statusMessage = "Created";
                    res.status(201).json( {userId: user_id } );
                } catch (err) {
                    if (err.length > 1) {
                        res.statusMessage = 'Error: ' + err[1];
                        res.status( err[0] ).send();
                    } else {
                        console.log(err);
                        res.status( 500 ).send( err );
                    }
                }
            } else {
                res.statusMessage = "Error: Bad Request: data should have required property 'password'";
                res.status( 400 ).send();
            }
        } else {
            res.statusMessage = "Error: Bad Request: data should have required property 'email'";
            res.status( 400 ).send();
        }
    } else {
        res.statusMessage = "Error: Bad Request: data should have required property 'name'";
        res.status( 400 ).send();
    }
};

exports.login = async function (req, res) {
    const email = req.body.email;
    const password = req.body.password;
    try {
        if (password) {
            const result = await user.login(email, password);
            res.statusMessage = "OK";
            res.status(200).json( { userId: result[0], token: result[1] } );
        } else {
            res.statusMessage = 'Bad request: data should have required property "password"';
            res.status( 400 ).send();
        }
    } catch (err) {
        if (err.length > 1) {
            res.statusMessage = 'Error: ' + err[1];
            res.status( err[0] ).send();
        } else {
            console.log(err);
            res.status( 500 ).send( err );
        }
    }
};

exports.logout = async function (req, res) {
    const auth = req.headers["x-authorization"] || null;
    try {
        await user.logout(auth);
        res.statusMessage = "OK";
        res.status(200).send();
    } catch (err) {
        if (err.length > 1) {
            res.statusMessage = 'Error: ' + err[1];
            res.status( err[0] ).send();
        } else {
            console.log(err);
            res.status( 500 ).send( err );
        }
    }
};

exports.get = async function (req, res) {
    const id = req.params.id;
    const auth = req.headers["x-authorization"] || null;
    try {
        const result = await user.get( id, auth );
        if( !result ){
            res.statusMessage = 'Error: Not found';
            res.status( 404 ).send();
        } else {
            res.statusMessage = "OK";
            res.status( 200 ).json( result );
        }
    } catch( err ) {
        if (err.length > 1) {
            res.statusMessage = 'Error: ' + err[1];
            res.status( err[0] ).send();
        } else {
            console.log(err);
            res.status( 500 ).send( err );
        }
    }
};

exports.update = async function (req, res) {
    const id = req.params.id;
    const auth = req.headers["x-authorization"] || null;
    const name = req.body.name || null ;
    const email = req.body.email || null;
    const password = req.body.password || null;
    const currentPassword = req.body.currentPassword || null;
    const city = req.body.city || null;
    const country = req.body.country || null;

    try {
        const result = await user.update( id, auth, name, email, password, currentPassword, city, country );
        res.statusMessage = "OK";
        res.status(200).send();
    } catch (err) {
        if (err.length > 1) {
            res.statusMessage = 'Error: ' + err[1];
            res.status( err[0] ).send();
        } else {
            console.log(err);
            res.status( 500 ).send( err );
        }
    }
};

exports.getPhoto = async function (req, res) {
    const id = req.params.id;
    try {
        const filename = await user.getPhoto( id );
        if (await fs.exists(photoDirectory + filename)) {
            const image = await fs.readFile(photoDirectory + filename);
            const extension = filename.split(".")[1];
            let mimeType = "image/jpeg";
            if (extension == "jpg" || extension == "jpeg") {
                mimeType = "image/jpeg";
            } else if (extension == "png") {
                mimeType = "image/png";
            } else if (extension == "gif") {
                mimeType = "image/gif";
            }
            res.statusMessage = "OK";
            res.status( 200 ).contentType(mimeType).send(image);
        } else {
            res.statusMessage = 'Error: Not found';
            res.status( 404 ).send();
        }
    } catch( err ) {
        if (err.length > 1) {
            res.statusMessage = 'Error: ' + err[1];
            res.status( err[0] ).send();
        } else {
            console.log(err);
            res.statusMessage = err;
            res.status( 500 ).send();
        }
    }
};


exports.setPhoto = async function (req, res) {
    const id = req.params.id;
    const auth = req.headers["x-authorization"] || null;
    try {
        let fileExt = null;
        if ( req.header('content-type') == 'image/jpeg' ) {
            fileExt = '.jpg';
        } else if ( req.header('content-type') == 'image/png' ) {
            fileExt = '.png';
        } else if ( req.header('content-type') == 'image/gif' ) {
            fileExt = '.gif';
        }
        if (fileExt === null) {
            res.statusMessage = 'Bad Request: photo must be either image/jpeg or image/png type or image/gif type';
            res.status(400).send();
        }

        const result = await user.setPhoto( id, auth, req.body, fileExt );
        if (result == 200) {
            res.statusMessage = "OK";
            res.status( 200 ).send();
        } else {
            res.statusMessage = "Created";
            res.status( 201 ).send();
        }
    } catch( err ) {
        if (err.length > 1) {
            res.statusMessage = 'Error: ' + err[1];
            res.status( err[0] ).send();
        } else {
            console.log(err);
            res.status( 500 ).send( err );
        }
    }
};

exports.removePhoto = async function (req, res) {
    const id = req.params.id;
    const auth = req.headers["x-authorization"] || null;
    try {
        await user.removePhoto( id, auth );
        res.statusMessage = "OK";
        res.status( 200 ).send();
    } catch( err ) {
        if (err.length > 1) {
            res.statusMessage = 'Error: ' + err[1];
            res.status( err[0] ).send();
        } else {
            console.log(err);
            res.status( 500 ).send( err );
        }
    }
};