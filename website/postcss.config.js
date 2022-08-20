let postCssConfig = {
  plugins: {
    'postcss-import': {},
    autoprefixer: {},
  }
};

if (process.env.NODE_ENV === 'production') {
  postCssConfig.plugins.cssnano = {};
}

module.exports = postCssConfig;
