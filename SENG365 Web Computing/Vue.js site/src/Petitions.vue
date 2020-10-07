<template>
  <div>
    <div class="alert alert-danger" v-if="errorFlag">
      {{ error }}
    </div>
    <div class="petitions container">
      <div class="filters row">
        <div class="col-sm-5">
          <label class="text-muted" style="color: white">.</label>
          <form class="search" v-on:submit.prevent="getPetitions()">
            <div class="form-group col-sm-9">
              <input class="form-control" v-model="search" placeholder="Search"/>
            </div>
            <button type="submit" class="btn btn-secondary col-sm-3">Search</button>
          </form>
        </div>
        <div class="col-sm-4 form-group">
          <label for="category" class="text-muted">Category:</label>
          <select class="form-control" v-model="category" id="category">
            <option value="all">All</option>
            <option v-for="category in categories.data"
                    v-bind:value="category.categoryId">{{category.name}}</option>
          </select>
        </div>
        <div class="col-sm-3 form-group">
          <label for="sort" class="text-muted">Sort:</label>
          <select class="form-control" v-model="sort" id="sort">
            <option v-for="sort in sorts"
                    v-bind:value="sort.sortId">{{sort.name}}</option>
          </select>
        </div>

        <div class="row" v-for="petition in petitions.slice(count*currentPage-count, count*currentPage)">
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
        <div class="pagination-footer col-sm-12">
          <ul class="pagination">
            <li class="previous" :class="{'disabled' : currentPage == 1}"><a v-on:click="goto(1)">First</a></li>
            <li class="previous" :class="{'disabled' : currentPage == 1}"><a v-on:click="goto(currentPage-1)">Previous</a></li>
            <li class="active" ><a >{{currentPage}}</a></li>
            <li class="next" :class="{'disabled' : currentPage >= lastPage()}"><a v-on:click="goto(currentPage+1)">Next</a></li>
            <li class="next" :class="{'disabled' : currentPage >= lastPage()}"><a v-on:click="goto(lastPage())">Last</a></li>
          </ul>
        </div>
      </div>
    </div>
  </div>
</template>

<script>
  export default {
    name: 'petitions',
    data (){
      return{
        petitions: [],
        errorFlag: false,
        error: "",
        search: "",
        categories: [],
        category: "all",
        sorts: [
          {sortId: 'ALPHABETICAL_ASC',
            name: 'Title A-Z'},
          {sortId: 'ALPHABETICAL_DESC',
            name: 'Title Z-A'},
          {sortId: 'SIGNATURES_ASC',
            name: 'Signatures increasing'},
          {sortId: 'SIGNATURES_DESC',
            name: 'Signatures decreasing'}
        ],
        sort: "SIGNATURES_DESC",
        count: 10,
        currentPage: 1
      }
    },
    mounted: function() {
      document.getElementById('reset').addEventListener("click", this.reset);
      document.getElementById('category').addEventListener("change", this.getPetitions);
      document.getElementById('sort').addEventListener("change", this.getPetitions);
      this.getPetitions();
      this.getCategories();
    },
    methods: {
      getPetitions: function () {
        let query = 'http://localhost:4941/api/v1/petitions';
        let params = [];
        if (this.search != "") {
          params.push('q=' + encodeURIComponent(this.search));
        }
        if (this.category != "all") {
          params.push('categoryId=' + this.category);
        }
        if (this.sort != "") {
          params.push('sortBy=' + this.sort);
        }
        for (let i=0; i < params.length; i++) {
          if (i==0) {
            query += '?' + params[i];
          } else {
            query += '&' + params[i];
          }
        }

        this.$http.get(query)
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
      reset: function() {
        this.search = "";
        this.category = "all";
        this.sort = "SIGNATURES_DESC";
        this.currentPage = 1;
        this.error = "";
        this.errorFlag = false;
        this.getPetitions();
      },
      lastPage: function() {
        return Math.floor(this.petitions.length/this.count) + (this.petitions.length%this.count == 0 ? 0 : 1);
      },
      goto: function(page) {
        if (page >= 1 && page <= Math.floor(this.petitions.length/this.count) + (this.petitions.length%this.count == 0 ? 0 : 1))
        this.currentPage = page;
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

  .search div {
    padding: 0;
    margin: 0;
  }

  label {
    font-size: 14px !important;
    font-weight: normal;
  }

  .pagination-footer {
    text-align: center;
  }

  .pagination a {
    cursor: pointer;
  }

  .pagination li {
    margin: 0;
  }

  .pagination .active a {
    cursor: default;
    margin: 0 10px;
  }
</style>
