<template>
  <div>
    <div class="alert alert-danger" v-if="errorFlag">
      {{ error }}
    </div>
    <div class="petition container">
      <h2 class="row col-sm-12 pet-title">Edit Petition</h2>
      <form v-on:submit.prevent="submit()">
        <div class="col-sm-12 pet-image"><img :src="petition.image"></div>
        <div class="form-group">
          <label for="heroImage">Change Hero image</label>
          <input id="heroImage" type="file" class="form-control" accept="image/png, image/jpeg, image/gif"/>
        </div>

        <div class="form-group">
          <label>Title</label>
          <input class="form-control" v-model="petition.title" required/>
        </div>
        <div class="form-group">
          <label>Description</label>
          <textarea class="form-control" v-model="petition.description" required/>
        </div>
        <div class="form-group">
          <label for="category">Category:</label>
          <select v-model="petition.categoryId" id="category" required>
            <option v-for="category in categories.data"
                    v-bind:value="category.categoryId">{{category.name}}</option>
          </select>
        </div>
        <div class="form-group">
          <label for="date">Closing date:</label>
          <input id="date" type="date" class="form-control" v-model="petition.closingDateShort"/>
        </div>
        <button type="submit" class="btn btn-primary">Save</button>
        <button type="button" v-on:click="deletePet()" class="btn btn-danger" style="float: right">Delete petition</button>
      </form>
    </div>
  </div>
</template>

<script>
  export default {
    name: 'edit-petition',
    data (){
      return{
        errorFlag: false,
        error: "",
        petition: [],
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
            this.getPetition();
            this.error = "";
            this.errorFlag = false;
          })
          .catch((error) => {
            if (error.response) this.error = error.response.statusText;
            else this.error = error;
            this.errorFlag = true;
          });
      },
      getPetition: function () {
        this.$http.get('http://localhost:4941/api/v1/petitions/' + this.$route.params.petitionId)
          .then((response) => {
            this.petition = response.data;
            this.petition.image = 'http://localhost:4941/api/v1/petitions/' + this.$route.params.petitionId + '/photo';

            if (this.petition.closingDate == null) {
              this.petition.closingDateShort = null;
            } else {
              const newdate = new Date(this.petition.closingDate);
              this.petition.closingDateShort = newdate.toLocaleDateString("en-CA");
            }

            this.getCategory();
            this.error = "";
            this.errorFlag = false;
          })
          .catch((error) => {
            if (error.response) this.error = error.response.statusText;
            else this.error = error;
            this.errorFlag = true;
          });
      },
      getCategory: function() {
        let i;
        for (i = 0; i < this.categories.data.length; i++) {
          if (this.petition.category == this.categories.data[i].name) {
            this.petition.categoryId = this.categories.data[i].categoryId;
          }
        }
      },
      submit: function() {
        const Image = document.getElementById("heroImage").files[0];
        if (Image) {
          if (['image/png', 'image/jpg', 'image/jpeg', 'image/gif'].indexOf(Image.type) < 0) {
            this.error = "Only gif, jpeg and png images are allowed.";
            this.errorFlag = true;
            return;
          } else {
            this.$http.put('http://localhost:4941/api/v1/petitions/' + this.$route.params.petitionId + '/photo',
              Image,
              {headers:
                {
                  'X-Authorization': this.getCookieValue("Token"),
                  'Content-Type': Image.type
                }
              }
            )
            .then(
              this.submitData()
            )
            .catch((error) => {
              if (error.response) this.error = error.response.statusText;
              else this.error = error;
              this.errorFlag = true;
            });
          }
        } else {
          this.submitData();
        }
      },
      submitData: function() {
        const data = {
          "title": this.petition.title,
          "description": this.petition.description,
          "categoryId": parseInt(this.petition.categoryId)
        };
        if (this.petition.closingDateShort != null) {
          data["closingDate"] = this.petition.closingDateShort;
        }

        this.$http.patch('http://localhost:4941/api/v1/petitions/' +  this.$route.params.petitionId, data,
          { headers:
            { 'X-Authorization': this.getCookieValue("Token")}
          })
          .then((response) => {
            this.$router.push({name: 'petition', params: {petitionId: this.$route.params.petitionId }})
              .then((response) => {
                this.error = "";
                this.errorFlag = false;
              })
              .catch((err) => {
                  this.error = error;
                  this.errorFlag = true;
                })
              })
          .catch((error) => {
            if (error.response) this.error = error.response.statusText;
            else this.error = error;
            this.errorFlag = true;
          });
      },
      deletePet: function() {
        this.$http.delete('http://localhost:4941/api/v1/petitions/' + this.$route.params.petitionId,
          {headers:
            {'X-Authorization': this.getCookieValue("Token"),
              'Content-Type': Image.type }
          }
        )
          .then(
            this.$router.push({name: 'my-petitions'})
              .then((response) => {
                this.error = "";
                this.errorFlag = false;
              })
              .catch((err) => {
                this.error = error;
                this.errorFlag = true;
              })
          )
          .catch((error) => {
            if (error.response) this.error = error.response.statusText;
            else this.error = error;
            this.errorFlag = true;
          });
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
