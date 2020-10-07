const petitions = require('../controllers/petitions.controller');

module.exports = function (app) {
    app.route(app.rootUrl + '/petitions')
        .get(petitions.getAll)
        .post(petitions.create);

    app.route(app.rootUrl + '/petitions/categories')
        .get(petitions.getCategories);

    app.route(app.rootUrl + '/petitions/:id')
        .get(petitions.getOne)
        .patch(petitions.update)
        .delete(petitions.remove);

    app.route(app.rootUrl + '/petitions/:id/photo')
        .get(petitions.getPhoto)
        .put(petitions.setPhoto);

    app.route(app.rootUrl + '/petitions/:id/signatures')
        .get(petitions.getSignatures)
        .post(petitions.sign)
        .delete(petitions.unsign);
};