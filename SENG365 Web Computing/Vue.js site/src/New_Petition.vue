<template>
  <div>
    <div class="alert alert-danger" v-if="errorFlag">
      {{ error }}
    </div>
    <div class="petition container">
      <h2 class="row col-sm-12 pet-title">Create Petition</h2>
      <form v-on:submit.prevent="create()">
        <div class="form-group">
          <input class="form-control" v-model="title" placeholder="Title" required/>
        </div>
        <div class="form-group">
          <textarea class="form-control" v-model="description" placeholder="Description" required/>
        </div>
        <div class="form-group">
          <label for="category">Category:</label>
          <select v-model="categoryId" id="category" required>
            <option v-for="category in categories.data"
                    v-bind:value="category.categoryId">{{category.name}}</option>
          </select>
        </div>
        <div class="form-group">
          <label for="heroImage">Choose Hero image</label>
          <input id="heroImage" type="file" class="form-control" accept="image/png, image/jpeg, image/gif" required/>
        </div>
        <div class="form-group">
          <label for="date">Closing date:</label>
          <input id="date" type="date" class="form-control" v-model="closingDate" placeholder="Closing date"/>
        </div>
        <button type="submit" class="btn btn-primary">Create</button>
      </form>
    </div>
  </div>
</template>

<script>
  export default {
    name: 'new-petition',
    data (){
      return{
        errorFlag: false,
        error: "",
        title: "",
        description: "",
        categoryId: "",
        closingDate: "",
        petitionId: "",
        categories: []
      }
    },
    props: {
      getCookieValue: Function
    },
    mounted: function() {
      this.getCategories();
      let  now = new Date();
      const minDate = now.toISOString().substring(0,10);
      let max = new Date('9999-12-31');
      const maxDate = max.toISOString().substring(0,10);
      $('#date').prop('min', minDate);
      $('#date').prop('max', maxDate);
    },
    methods: {
      getCategories: function() {
        this.$http.get('http://localhost:4941/api/v1/petitions/categories')
          .then((response) => {
            this.categories = response;
            this.error = "";
            this.errorFlag = false;
          })
          .catch((error) => {
            if (error.response) this.error = error.response.statusText;
            else this.error = error;
            this.errorFlag = true;
          });
      },
      create: function() {
        const Image = document.getElementById("heroImage").files[0];
        if (['image/png', 'image/jpg', 'image/jpeg', 'image/gif'].indexOf(Image.type) < 0) {
          this.error = "Only gif, jpeg and png images are allowed.";
          this.errorFlag = true;
        } else {
          const data = {"title": this.title, "description": this.description, "categoryId": parseInt(this.categoryId)};
          if (this.closingDate.length > 0) {
            data["closingDate"] = this.closingDate;
          }

          this.$http.post('http://localhost:4941/api/v1/petitions', data,
            { headers:
              { 'X-Authorization': this.getCookieValue("Token")}
            })
            .then((response) => {
              this.petitionId = response.data.petitionId;
              this.$http.put(
                'http://localhost:4941/api/v1/petitions/' + this.petitionId + '/photo',
                Image,
                { headers:
                  { 'X-Authorization': this.getCookieValue("Token"),
                    'Content-Type': Image.type}
                }
              )
                .then (
                  this.$http.post('http://localhost:4941/api/v1/petitions/' + this.petitionId + '/signatures', {},
                    { headers:
                      { 'X-Authorization': this.getCookieValue("Token")}
                    }
                  )
                    .then ( this.$router.push({name: 'petition', params: {petitionId: this.petitionId}})
                      .then((response) => {
                        this.error = "";
                        this.errorFlag = false;
                      })
                      .catch((error) => {
                        if (error.response) this.error = error.response.statusText;
                        else this.error = error;
                        this.errorFlag = true;
                      })
                    )
                    .catch((error) => {
                      if (error.response) this.error = error.response.statusText;
                      else this.error = error;
                      this.errorFlag = true;
                    })
                )
                .catch((error) => {
                  if (error.response) this.error = error.response.statusText;
                  else this.error = error;
                  this.errorFlag = true;
                });
            })
            .catch((error) => {
              if (error.response) this.error = error.response.statusText;
              else this.error = error;
              this.errorFlag = true;
            });
        }
      }
    }
  }
</script>

<style scoped>
  .text-muted {
    font-size: 10px;
    display: inline-block;
  }

  .pet-image img {
    max-width: 100%;
  }

  .row {
    padding-top: 5px;
    padding-bottom: 10px;
  }

  h4.row {
    padding-top: 40px;
  }

  img.default {
    display: none;
  }

  .error img {
    display: none;
  }

  .error img.default {
    display: block;
  }
</style>
