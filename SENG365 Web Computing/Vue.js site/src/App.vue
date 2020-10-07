<template>
  <div id="app" class="container-fluid">
    <template>
      <div>
        <div class="alert alert-danger" v-if="errorFlag">
          {{ error }}
        </div>
        <div class="nav container">
          <button id="reset" v-if="$route.name == 'petitions'" type="button" class="btn btn-default" style="float: left">All Petitions</button>
          <router-link v-else :to="{ name: 'petitions'}" class="btn btn-default" style="float: left">All Petitions</router-link>

          <template v-if="loggedIn">
            <router-link :to="{ name: 'new-petition'}" class="btn btn-default">Create Petition</router-link>
            <router-link :to="{ name: 'my-petitions'}" class="btn btn-default">My Petitions</router-link>
            <router-link :to="{ name: 'profile'}" class="btn btn-default">Profile</router-link>
            <button type="button" class="btn btn-default" v-on:click="function(event){logout(event)}">
              Log Out
            </button>
          </template>
          <template v-else>
            <button type="button" class="btn btn-light" data-toggle="modal" data-target="#registerModal">Register</button>
            <button type="button" class="btn btn-default" data-toggle="modal" data-target="#loginModal">
              Log In
            </button>
          </template>
        </div>

        <div class="modal fade" id="loginModal" tabindex="-1" role="dialog"
             aria-labelledby="loginModalLabel" aria-hidden="true">
          <div class="modal-dialog" role="document">
            <div class="modal-content">
              <div class="modal-header">
                <h5 class="modal-title" id="loginModalLabel">Log In</h5>
                <button type="button" class="close" data-dismiss="modal" aria-label="Close">
                  <span aria-hidden="true">&times;</span>
                </button>
              </div>
              <div class="modal-body">
                <form v-on:submit.prevent="loginForm()">
                  <div class="form-group">
                    <input class="form-control" v-model="email" placeholder="Email address" required/>
                  </div>
                  <div class="form-group">
                    <input class="form-control" type = "password" v-model="password" placeholder="Password" required/>
                  </div>
                  <button type="submit" class="btn btn-primary">Log In</button>
                </form>
                <div v-if="formFlag" class="alert alert-danger">
                  {{ formError }}
                </div>
              </div>
              <div class="modal-footer">
                <button type="button" class="btn btn-info" data-dismiss="modal" data-toggle="modal" data-target="#registerModal">Register</button>
                <button type="button" class="btn btn-secondary" data-dismiss="modal">
                  Close
                </button>
              </div>
            </div>
          </div>
        </div>

        <div class="modal fade" id="registerModal" tabindex="-1" role="dialog"
             aria-labelledby="registerModalLabel" aria-hidden="true">
          <div class="modal-dialog" role="document">
            <div class="modal-content">
              <div class="modal-header">
                <h5 class="modal-title" id="registerModalLabel">Register</h5>
                <button type="button" class="close" data-dismiss="modal" aria-label="Close">
                  <span aria-hidden="true">&times;</span>
                </button>
              </div>
              <div class="modal-body">
                <form v-on:submit.prevent="register()">
                  <div class="form-group">
                    <input class="form-control" v-model="name" placeholder="Name" required/>
                  </div>
                  <div class="form-group">
                    <input class="form-control" v-model="email" placeholder="Email address" required/>
                  </div>
                  <div class="form-group">
                    <input class="form-control" type = "password" v-model="password" placeholder="Password" required/>
                  </div>
                  <div class="form-group">
                    <input class="form-control" v-model="city" placeholder="City"/>
                  </div>
                  <div class="form-group">
                    <input class="form-control" v-model="country" placeholder="Country"/>
                  </div>
                  <div class="form-group">
                    <label for="image" class="btn">Choose Profile Picture</label>
                    <input id="image" type="file" class="form-control" accept="image/png, image/jpeg, image/gif"/>
                  </div>
                  <button type="submit" class="btn btn-primary">Register</button>
                </form>
                <div v-if="formFlag" class="alert alert-danger">
                  {{ formError }}
                </div>
              </div>
              <div class="modal-footer">
                <button type="button" class="btn btn-secondary" data-dismiss="modal">
                  Close
                </button>
              </div>
            </div>
          </div>
        </div>
      </div>
    </template>

    <router-view :logged-in="loggedIn" :get-cookie-value="getCookieValue" :validate-email="validateEmail"></router-view>
  </div>
</template>

<script>
  export default {
    name: 'app',
    data (){
      return{
        errorFlag: false,
        error: "",
        formError: "",
        formFlag: false,
        name: "",
        email: "",
        password: "",
        city: "",
        country: "",
        loggedIn: false
      }
    },
    mounted: function() {
      this.isLoggedIn();
    },
    methods: {
      isLoggedIn: function () {
        if (this.getCookieValue("Token") != "") {
          this.loggedIn = true;
        } else {
          this.loggedIn = false;
        }
      },
      loginForm: function () {
        if (!this.validateEmail(this.email)) {
          this.formError = "Email address is not the correct format.";
          this.formFlag = true;
        } else if (this.password === "") {
          this.formError = "Please enter a password!";
          this.formFlag = true;
        } else {
          this.formFlag = false;
          this.login();
        }
      },
      login: function () {
        this.$http.post('http://localhost:4941/api/v1/users/login', {
          "email": this.email,
          "password": this.password
        })
          .then((response) => {
            document.cookie = "Token=" + response.data.token;
            document.cookie = "UserId=" + response.data.userId;
            $('#loginModal').modal('hide');
            this.loggedIn = true;
            this.formError = "";
            this.formFlag = false;
          })
          .catch((error) => {
            if (error.response) this.formError = error.response.statusText;
            else this.formError = error;
            this.formFlag = true;
          });
      },
      logout: function (event) {
        event.stopPropagation();
        document.cookie = "Token=;expires=Thu, 01 Jan 1970 00:00:01 GMT";
        document.cookie = "UserId=;expires=Thu, 01 Jan 1970 00:00:01 GMT";
        this.loggedIn = false;
        if (this.$route.name == 'edit-petition') {
          this.$router.push({name: 'petition', params: {petitionId: this.$route.params.petitionId }})
            .then((response) => {
              this.formError = "";
              this.formFlag = false;
            })
            .catch((err) => {
              this.error = error;
              this.errorFlag = true;
            })
        } else if (['new-petition','my-petitions', 'edit-profile', 'profile'].indexOf(this.$route.name) >= 0 ) {
          this.$router.push({name: 'petitions'})
            .then((response) => {
              this.formError = "";
              this.formFlag = false;
            })
            .catch((err) => {
              this.error = error;
              this.errorFlag = true;
            })
        }
      },
      register: function () {
        const Image = document.getElementById("image").files[0];
        if (!this.validateEmail(this.email)) {
          this.formError = "Email address is not the correct format.";
          this.formFlag = true;
        } else if (Image) {
          if (['image/png', 'image/jpg', 'image/jpeg', 'image/gif'].indexOf(Image.type) < 0) {
            this.formError = "Only gif, jpeg and png images are allowed.";
            this.formFlag = true;
          }
        }
        const data = {"name": this.name, "email": this.email, "password": this.password};
        this.formFlag = false;
        if (this.city.length > 0) {
          data["city"] = this.city;
        }
        if (this.country.length > 0) {
          data["country"] = this.country;
        }

        this.$http.post('http://localhost:4941/api/v1/users/register', data)
          .then((response) => {
            this.$http.post('http://localhost:4941/api/v1/users/login', {
              "email": this.email,
              "password": this.password
            })
              .then((response) => {
                document.cookie = "Token=" + response.data.token;
                document.cookie = "UserId=" + response.data.userId;
                $('#loginModal').modal('hide');
                if (Image) {
                  this.$http.put( 'http://localhost:4941/api/v1/users/' + this.getCookieValue("UserId") + '/photo',
                    Image,
                    {headers:
                      {'X-Authorization': this.getCookieValue("Token"),
                        'Content-Type': Image.type}
                    }
                  )
                    .catch((error) => {
                      if (error.response) this.formError = error.response.statusText;
                      else this.formError = error;
                      this.formFlag = true;
                    });
                }
                this.loggedIn = true;
                $('#registerModal').modal('hide');
                this.formError = "";
                this.formFlag = false;
              })
              .catch((error) => {
                if (error.response) this.formError = error.response.statusText;
                else this.formError = error;
                this.formFlag = true;
              });
          })
          .catch((error) => {
            if (error.response) this.formError = error.response.statusText;
            else this.formError = error;
            this.formFlag = true;
          });
      },
      validateEmail: function (email) {
        if (/^\w+([\.-]?\w+)*@\w+([\.-]?\w+)*(\.\w{2,3})+$/.test(email))
        {
          return (true)
        }
        return (false)
      },
      getCookieValue: function (key) {
        const value = document.cookie.match('(^|[^;]+)\\s*' + key + '\\s*=\\s*([^;]+)');
        return value ? value.pop() : '';
      }
    }
  }
</script>/*

<style>
#app {
  font-family: 'Avenir', Helvetica, Arial, sans-serif;
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
  color: #2c3e50;
  margin: 10px;
}

h1, h2 {
  font-weight: normal;
}

ul {
  list-style-type: none;
  padding: 0;
}

li {
  display: inline-block;
  margin: 0 10px;
}

a {
  color: #42b983;
}

.nav {
  text-align: right;
  margin-bottom: 10px;
}
</style>
