<template>
  <div>
    <div class="alert alert-danger" v-if="errorFlag">
      {{ error }}
    </div>
    <div class="profile container">
      <h2 class="row col-sm-10">Profile</h2>
      <router-link :to="{ name: 'edit-profile'}" class="btn btn-primary" style="float:right; margin-top: 20px">Edit</router-link>
      <div class="row">
        <div class="col-sm-4 text-muted">Profile picture:</div>
        <div class="col-sm-8 profile-image">
          <img id="profile-image" :src="profile.image" onerror="this.onerror=null;this.parentElement.classList.add('error');">
          <img class="default" src="./assets/default.png">
        </div>
      </div>
      <div class="row">
        <div class="col-sm-4 text-muted">Name:</div>
        <div class="col-sm-8">{{ profile.name }}</div>
      </div>
      <div class="row">
        <div class="col-sm-4 text-muted">Email:</div>
        <div class="col-sm-8">{{ profile.email }}</div>
      </div>
      <div class="row">
        <div class="col-sm-4 text-muted">City:</div>
        <div class="col-sm-8">{{ profile.city }}</div>
      </div>
      <div class="row">
        <div class="col-sm-4 text-muted">Country:</div>
        <div class="col-sm-8">{{ profile.country }}</div>
      </div>
    </div>
  </div>
</template>

<script>
  export default {
    name: 'profile',
    data (){
      return{
        profile: [],
        errorFlag: false,
        error: "",
      }
    },
    props: {
      getCookieValue: Function
    },
    mounted: function() {
      this.getProfile();
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
            this.reloadImage();
          })
          .catch((error) => {
            if (error.response) this.error = error.response.statusText;
            else this.error = error;
            this.errorFlag = true;
          });
      },
      reloadImage: function() {
        setTimeout(function() {
          const url = $('#profile-image').attr("src");
          $('#petition-image').attr("src", url + '?');
        },200);
      }
    }
  }
</script>

<style scoped>
  .row {
    padding-top: 5px;
    padding-bottom: 10px;
  }

  .profile-image img {
    max-width: 100%;
    width: 200px
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
