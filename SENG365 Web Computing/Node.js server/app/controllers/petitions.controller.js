const petition = require('../models/petitions.model');
const fs = require('mz/fs');
const photoDirectory = './storage/photos/';

function isInt(value) {
    return !isNaN(value) &&
        parseInt(Number(value)) == value &&
        !isNaN(parseInt(value, 10));
}

exports.getAll = async function (req, res) {
    const startIndex = parseInt(req.query.startIndex) || 0;
    const count = parseInt(req.query.count);
    try {
        if ( isInt(startIndex) ) {
            const result = await petition.getAll(req.query.q, req.query.categoryId, req.query.authorId, req.query.sortBy);
            if (count) {
                if (isInt(count)) {
                    res.status(200).send(result.slice(startIndex, startIndex + count));
                } else {
                    res.statusMessage = 'Bad Request: data.count should be an integer';
                    res.status(400).send();
                }
            } else {
                res.status(200).send(result.slice(startIndex));
            }
        } else {
            res.statusMessage = 'Bad Request: data.startIndex should be an integer';
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

exports.create = async function (req, res) {
    const auth = req.headers["x-authorization"] || null;
    const title = req.body.title;
    const description = req.body.description;
    const categoryId = req.body.categoryId;
    const closingDate = req.body.closingDate || null;
    if (title) {
        if (description) {
            if (categoryId) {
                try {
                    const result = await petition.insert(auth, title, description, categoryId, closingDate);
                    res.statusMessage = "Created";
                    res.status(201).json({petitionId: result});
                } catch (err) {
                    if (err.length > 1) {
                        res.statusMessage = 'Error: ' + err[1];
                        res.status(err[0]).send();
                    } else {
                        console.log(err);
                        res.status(500).send(err);
                    }
                }
            } else {
                res.statusMessage = "Error: Bad Request: data should have required property 'categoryId'";
                res.status( 400 ).send();
            }
        } else {
            res.statusMessage = "Error: Bad Request: data should have required property 'description'";
            res.status( 400 ).send();
        }
    } else {
        res.statusMessage = "Error: Bad Request: data should have required property 'title'";
        res.status( 400 ).send();
    }
};

exports.getOne = async function (req, res) {
    const id = req.params.id;
    try {
        const result = await petition.getOne( id );
        if( !result ){
            res.statusMessage = 'Error: Not found';
            res.status( 404 ).send();
        } else {
            res.statusMessage = "OK";
            res.status( 200 ).json( result[0] );
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

exports.update = async function (req, res) {
    const auth = req.headers["x-authorization"] || null;
    const id = req.params.id;
    const title = req.body.title || null;
    const description = req.body.description || null;
    const categoryId = req.body.categoryId || null;
    const closingDate = req.body.closingDate || null;
    try {
        await petition.update( auth, id, title, description, categoryId, closingDate );
        res.statusMessage = "OK";
        res.status( 200 ).send();
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

exports.remove = async function (req, res) {
    const auth = req.headers["x-authorization"] || null;
    const id = req.params.id;
    try {
        await petition.remove( auth, id );
        res.statusMessage = "OK";
        res.status( 200 ).send();
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

exports.getCategories = async function (req, res) {
    try {
        const result = await petition.getCategories();
        res.statusMessage = "OK";
        res.status( 200 ).json( result );
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
        const filename = await petition.getPhoto( id );
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

        const result = await petition.setPhoto( id, auth, req.body, fileExt );
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

exports.getSignatures = async function (req, res) {
    const id = req.params.id;
    try {
        const result = await petition.getSignatures(id);
        res.statusMessage = "OK";
        res.status( 200 ).json( result );
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

exports.sign = async function (req, res) {
    const id = req.params.id;
    const auth = req.headers["x-authorization"] || null;
    try {
        await petition.sign(id, auth);
        res.statusMessage = "Created";
        res.status( 201 ).send();
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

exports.unsign = async function (req, res) {
    const id = req.params.id;
    const auth = req.headers["x-authorization"] || null;
    try {
        await petition.unsign(id, auth);
        res.statusMessage = "OK";
        res.status( 200 ).send();
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