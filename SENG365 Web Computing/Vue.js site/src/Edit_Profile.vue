<template>
  <div>
    <div class="alert alert-danger" v-if="errorFlag">
      {{ error }}
    </div>
    <div class="petition container">
      <h2 class="row col-sm-12 pet-title">Edit Profile</h2>
      <form v-on:submit.prevent="submit()">
        <div class="col-sm-12 pet-image">
          <img :src="profile.image" onerror="this.onerror=null;this.parentElement.classList.add('error');">
          <img class="default" src="./assets/default.png">
        </div>
        <div class="form-group">
          <label for="Image" class="col-sm-6">Change Profile Picture</label>
          <a class="col-sm-6" v-on:click="deleteImage()">Delete profile picture</a>
          <input id="Image" type="file" class="form-control" accept="image/png, image/jpeg, image/gif"/>
        </div>

        <div class="form-group">
          <label>Name</label>
          <input class="form-control" v-model="profile.name" required/>
        </div>
        <div class="form-group">
          <label>Email</label>
          <input class="form-control" v-model="profile.email" required/>
        </div>
        <div class="form-group">
          <label>City:</label>
          <input class="form-control" v-model="profile.city"/>
        </div>
        <div class="form-group">
          <label>Country:</label>
          <input class="form-control" v-model="profile.country"/>
        </div>
        <div class="form-group">
          <label for="currentPassword">Current Password:</label>
          <input type="password" id="currentPassword" class="form-control" v-model="currentPassword"/>
        </div>
        <div class="form-group">
          <label for="password">New Password:</label>
          <input type="password" id="password" class="form-control" v-model="password"/>
        </div>
        <button type="submit" class="btn btn-primary">Save</button>
      </form>
    </div>
  </div>
</template>

<script>
  export default {
    name: 'edit-profile',
    data (){
      return{
        errorFlag: false,
        error: "",
        profile: [],
        password: "",
        currentPassword: "",
        delete: false
      }
    },
    props: {
      getCookieValue: Function,
      validateEmail: Function
    },
    mounted: function() {
      this.getProfile();
      $('#password').on('change', function() {
        if (this.password != "") {
          $('#currentPassword').prop('required', true);
        }
      })
    },
    methods: {
      getProfile: function () {
        this.$http.get('http://localhost:4941/api/v1/users/' + this.getCookieValue("UserId"),
          { headers: { 'X-Authorization': this.getCookieValue("Token") }
          })
          .then((response) => {
            this.profile = response.data;
            this.profile.image = 'http://localhost:4941/api/v1/users/' + this.getCookieValue("UserId") + '/photo';
            this.error = "";
            this.errorFlag = false;
          })
          .catch((error) => {
            if (error.response) this.error = error.response.statusText;
            else this.error = error;
            this.errorFlag = true;
          });
      },
      submit: function() {
        const Image = document.getElementById("Image").files[0];
        if (Image) {
          if (['image/png', 'image/jpg', 'image/jpeg', 'image/gif'].indexOf(Image.type) < 0) {
            this.error = "Only gif, jpeg and png images are allowed.";
            this.errorFlag = true;
          } else {
            this.$http.put('http://localhost:4941/api/v1/users/' + this.getCookieValue('UserId') + '/photo',
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
        if (!this.validateEmail(this.profile.email)) {
          this.error = "Email address is not the correct format.";
          this.errorFlag = true;
        } else {
          const data = {"name": this.profile.name, "email": this.profile.email};
          if (this.profile.city && this.profile.city.length > 0) {
            data["city"] = this.profile.city;
          }
          if (this.profile.country && this.profile.country.length > 0) {
            data["country"] = this.profile.country;
          }
          if (this.password.length > 0) {
            data["password"] = this.password;
          }
          if (this.currentPassword.length > 0) {
            data["currentPassword"] = this.currentPassword;
          }

          this.$http.patch('http://localhost:4941/api/v1/users/' + this.getCookieValue('UserId'), data,
            {headers:
              {'X-Authorization': this.getCookieValue("Token")}
            })
            .then((response) => {
              if (this.delete) {
                this.$http.delete('http://localhost:4941/api/v1/users/' + this.getCookieValue('UserId') + '/photo',
                  {headers:
                    {'X-Authorization': this.getCookieValue("Token")}
                  }
                )
                  .then((response) => {
                    this.$router.push({name: 'profile'})
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
              } else {
                this.$router.push({name: 'profile'})
                  .then((response) => {
                    this.error = "";
                    this.errorFlag = false;
                  })
                  .catch((err) => {
                    this.error = error;
                    this.errorFlag = true;
                  })
              }
            })
            .catch((error) => {
              if (error.response) this.error = error.response.statusText;
              else this.error = error;
              this.errorFlag = true;
            });
        }
      },
      deleteImage: function() {
        $('.pet-image').addClass('error');
        $('#Image').val("");
        this.delete = true;
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
    width: 200px
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
