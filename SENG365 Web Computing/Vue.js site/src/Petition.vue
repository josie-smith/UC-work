<template>
  <div>
    <div class="alert alert-danger" v-if="errorFlag">
      {{ error }}
    </div>
    <div class="petition container">
      <div class="row">
        <div class="edit-bar">
          <router-link v-if='petition.authorId == getCookieValue("UserId")' :to="{ name: 'edit-petition', params: { petitionId: this.$route.params.petitionId }}" class="btn btn-primary">Edit</router-link>
        </div>

        <div class="col-sm-12 pet-image"><img id="petition-image" :src="petition.image"></div>
        <h2 class="row col-sm-12 pet-title">{{ petition.title }}</h2>
        <div class="row col-sm-12">{{ petition.description }}</div>
        <div class="row col-sm-6">
          <div class="col-sm-4 text-muted">Category:</div>
          <div class="col-sm-8">{{ petition.category }}</div>
        </div>
        <div class="row col-sm-6">
          <div class="col-sm-4 text-muted">Signatures:</div>
          <div class="col-sm-8">{{ petition.signatureCount }}</div>
        </div>
        <div class="row col-sm-6">
          <div class="col-sm-4 text-muted">Created:</div>
          <div class="col-sm-8">{{ petition.createdDate | formatDate }}</div>
        </div>
        <div class="row col-sm-6">
          <div class="col-sm-4 text-muted">Closing:</div>
          <div class="col-sm-8">{{ petition.closingDate | formatDate }}</div>
        </div>

        <div class="row col-sm-12">
          <div class="col-sm-2 pet-image">
            <img :src="petition.authorImage" onerror="this.onerror=null;this.parentElement.classList.add('error');">
            <img class="default" src="./assets/default.png">
          </div>
          <div class="col-sm-10">
            <div class="col-sm-12 text-muted">Author:</div>
            <div class="col-sm-12">{{ petition.authorName }}</div>
            <div class="col-sm-12">{{ petition.authorCity }}</div>
            <div class="col-sm-12">{{ petition.authorCountry }}</div>
          </div>
        </div>
      </div>

      <div class="row share">
        <div :data-href="'https://canterbury.ac.nz' + $route.path" >
          <a target="_blank" :href="'https://www.facebook.com/sharer/sharer.php?u=' + encodeURIComponent('https://canterbury.ac.nz' + $route.path) + '&amp;src=sdkpreparse'" class="fa fa-facebook"></a>
        </div>
        <a target="_blank" class="fa fa-twitter" :href="'https://twitter.com/intent/tweet?original_referer=' + encodeURIComponent('https://canterbury.ac.nz' + $route.path) + '&url=' + encodeURIComponent('https://canterbury.ac.nz' + $route.path) + '&text=' + encodeURIComponent(petition.title)"></a>
        <a target="_blank" class="fa fa-envelope" :href="'mailto:?subject=' + encodeURIComponent(petition.title) + '&amp;body=Check out this petition! ' + encodeURIComponent('https://canterbury.ac.nz' + $route.path)"></a>
      </div>

      <button v-if="showSignatures" type="button" class="btn btn-default" v-on:click="hideSignatories()">
        Hide Signatures
      </button>
      <button v-else type="button" class="btn btn-default" v-on:click="getSignatories()">
        Show Signatures
      </button>
      <div class="signatures">
        <div class="row heading">
          <h4 class="row col-sm-8">Signatures:</h4>
          <div v-if="loggedIn" class="col-sm-4 sign">
            <div v-if="new Date(petition.closingDate) > new Date() || petition.closingDate == null">
              <div v-if="signed">
                <button v-if='petition.authorId != getCookieValue("UserId")' type="button" class="btn btn-primary" v-on:click="unsign()">
                  Unsign
                </button>
                <div v-else class="text-muted">You can't remove a signature from your own petition.</div>
              </div>
              <button v-else type="button" class="btn btn-primary" v-on:click="sign()">
                Sign
              </button>
            </div>
            <div v-else class="text-muted">This petition has closed.</div>
          </div>
          <div v-else class="text-muted col-sm-4">Log in to sign this petition</div>
        </div>

        <div class="row" v-for="signature in signatures">
        <div class="col-sm-2 pet-image">
          <img :src="signature.image" onerror="this.onerror=null;this.parentElement.classList.add('error');">
          <img class="default" src="./assets/default.png">
        </div>
        <div class="col-sm-10">
          <div class="col-sm-12">{{ signature.name }}</div>
          <div class="col-sm-12">{{ signature.city }}</div>
          <div class="col-sm-12">{{ signature.country }}</div>
        </div>
      </div>
      </div>
    </div>
  </div>
</template>

<script>
  export default {
    name: 'petition',
    data (){
      return{
        petition: [],
        signatures: [],
        errorFlag: false,
        error: "",
        signed: false,
        showSignatures: false
      }
    },
    props: {
      loggedIn: Boolean,
      getCookieValue: Function
    },
    mounted: function() {
      this.getPetition();
    },
    filters: {
      formatDate: function (date) {
        if (date == null) {
          return "No closing date";
        } else {
          const newdate = new Date(date);
          return newdate.toLocaleDateString();
        }
      }
    },
    methods: {
      getPetition: function () {
        this.$http.get('http://localhost:4941/api/v1/petitions/' + this.$route.params.petitionId)
          .then((response) => {
            this.petition = response.data;
            this.petition.image = 'http://localhost:4941/api/v1/petitions/' + this.petition.petitionId + '/photo';
            this.petition.authorImage = 'http://localhost:4941/api/v1/users/' + this.petition.authorId + '/photo';
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
      getSignatories: function () {
        $('.signatures').show();
        this.showSignatures = true;
        this.$http.get('http://localhost:4941/api/v1/petitions/' + this.$route.params.petitionId + '/signatures')
          .then((response) => {
            this.signatures = response.data;
            let i;
            for (i=0; i < this.signatures.length; i++) {
              this.signatures[i].image = 'http://localhost:4941/api/v1/users/' + this.signatures[i].signatoryId + '/photo';
            }
            this.isSigned();
            this.error = "";
            this.errorFlag = false;
          })
          .catch((error) => {
            if (error.response) this.error = error.response.statusText;
            else this.error = error;
            this.errorFlag = true;
          });
      },
      hideSignatories: function () {
        $('.signatures').hide();
        this.showSignatures = false;
      },
      isSigned: function() {
        const userId = this.getCookieValue("UserId");
        for (const signature in this.signatures) {
          if (userId == this.signatures[signature].signatoryId) {
            this.signed = true;
          }
        }
      },
      sign: function() {
        this.$http.post('http://localhost:4941/api/v1/petitions/' + this.$route.params.petitionId + '/signatures', {},
          { headers: { 'X-Authorization': this.getCookieValue("Token") }
          }
        )
          .then((response) => {
            this.signed = true;
            this.getSignatories();
          })
          .catch((error) => {
            if (error.response) this.error = error.response.statusText;
            else this.error = error;
            this.errorFlag = true;
          });
      },
      unsign: function() {
        this.$http.delete('http://localhost:4941/api/v1/petitions/' + this.$route.params.petitionId + '/signatures',
          { headers: { 'X-Authorization': this.getCookieValue("Token") }
          }
        )
          .then((response) => {
            this.signed = false;
            this.getSignatories();
          })
          .catch((error) => {
            if (error.response) this.error = error.response.statusText;
            else this.error = error;
            this.errorFlag = true;
          });
      },
      reloadImage: function() {
        setTimeout(function() {
          const url = $('#petition-image').attr("src");
          $('#petition-image').attr("src", url + '?');
        },200);
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

  .heading {
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

  .sign, .sign div {
    float: right;
  }

  .share {
    text-align: right;
  }

  .share div {
    display: inline-block;
  }

  .fa {
    padding: 10px;
    font-size: 30px;
    width: 50px;
    text-align: center;
    text-decoration: none;
    border-radius: 50%;
    margin: 0 10px;
   }

  .fa:hover {
    opacity: 0.7;
  }

  .fa-facebook {
    background: #3B5998;
    color: white;
  }

  .fa-twitter {
    background: #55ACEE;
    color: white;
  }

  .fa-envelope {
    background: #eb4924;
    color: white;
  }

  .edit-bar {
    padding: 10px 0;
  }

  .signatures {
    display: none;
  }
</style>
