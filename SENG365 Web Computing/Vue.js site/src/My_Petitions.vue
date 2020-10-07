<template>
  <div>
    <div class="alert alert-danger" v-if="errorFlag">
      {{ error }}
    </div>
    <div class="petitions container">
      <h2 class="row col-sm-12">My Petitions</h2>
      <div class="row" v-for="petition in petitions">
        <div class="col-sm-6 pet-image"><img v-bind:src="petition.image"></div>
        <div class="col-sm-6 pet-content">
          <div class="col-sm-12 pet-title">{{ petition.title }}</div>
          <div class="col-sm-12">
            <div class="col-sm-4 text-muted">Author:</div>
            <div class="col-sm-8">{{ petition.authorName }}</div>
          </div>
          <div class="col-sm-12">
            <div class="col-sm-4 text-muted">Category:</div>
            <div class="col-sm-8">{{ petition.category }}</div>
          </div>
          <div class="col-sm-12">
            <div class="col-sm-4 text-muted">Signatures:</div>
            <div class="col-sm-8">{{ petition.signatureCount }}</div>
          </div>
          <div class="col-sm-12 pet-link"><router-link :to="{ name: 'petition', params: { petitionId: petition.petitionId }}">View</router-link></div>
        </div>
      </div>
    </div>
  </div>
</template>

<script>
  export default {
    name: 'my-petitions',
    data (){
      return{
        petitions: [],
        errorFlag: false,
        error: "",
      }
    },
    props: {
      getCookieValue: Function
    },
    mounted: function() {
      this.getPetitions();
    },
    methods: {
      getPetitions: function () {
        this.sleep(200);
        this.$http.get('http://localhost:4941/api/v1/petitions?authorId=' + this.getCookieValue('UserId'))
          .then((response) => {
            this.petitions = response.data;
            let i;
            for (i=0; i < this.petitions.length; i++) {
              this.petitions[i].image = 'http://localhost:4941/api/v1/petitions/' + this.petitions[i].petitionId + '/photo';
            }
            this.error = "";
            this.errorFlag = false;
          })
          .catch((error) => {
            if (error.response) this.error = error.response.statusText;
            else this.error = error;
            this.errorFlag = true;
          });
      },
      sleep: function(time) {
        var start = new Date().getTime();
        for (var i = 0; i < 1e7; i++) {
          if ((new Date().getTime() - start) > time) {
            break;
          }
        }
      }
    }
  }
</script>

<style scoped>
  .row {
    padding: 10px 0;
    margin: 5px 0;
  }

  .pet-image img {
    max-width: 100%;
  }

  .pet-title {
    font-size: 18px;
    padding-top: 5px;
    padding-bottom: 10px;
  }

  .text-muted {
    font-size: 10px;
    display: inline-block;
  }

  .pet-link {
    text-align: right;
  }

  label {
    font-size: 14px !important;
    font-weight: normal;
  }
</style>
