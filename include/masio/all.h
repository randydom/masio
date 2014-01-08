#ifndef __MASIO_ALL_H__
#define __MASIO_ALL_H__

namespace masio {

template<class A> class All : public Cont<std::vector<Error<A>>> {
public:
  typedef std::vector<Error<A>>       Result;
  typedef Cont<Result>                Super;
  typedef typename Super::CancelerPtr CancelerPtr;
  typedef typename Super::Rest        Rest;
  typedef typename Super::Run         Run;
  typedef typename Super::Ptr         Ptr;

  typedef typename Cont<A>::Ptr    SubPtr;

public:
  All(const SubPtr& c1, const SubPtr& c2)
    : _conts{c1, c2}
  {}

  void run(const CancelerPtr& canceler, const Rest& rest) const override {
    using namespace std;

    auto self      = Super::shared_from_this();

    auto remaining = make_shared<size_t>(_conts.size());
    auto results   = make_shared<Result>(*remaining);

    auto ci = _conts.begin();
    auto ri = results->begin();

    for (; ci != _conts.end(); ++ci, ++ri) {
      (*ci)->run(canceler, [self, remaining, results, ri, rest]
                           (const Error<A> e) {
          *ri = e;
          --*remaining;
          if (*remaining == 0) {
            rest(typename Error<Result>::Success{*results});
          }
          });
    }
  }

private:
  std::list<SubPtr> _conts;
};

template<class A>
typename All<A>::Ptr all( const typename Cont<A>::Ptr& a
                            , const typename Cont<A>::Ptr& b) {
  return std::make_shared<All<A>>(a, b);
}

} // masio namespace

#endif // ifndef __MASIO_ALL_H__
